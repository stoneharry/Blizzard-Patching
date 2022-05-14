# Blizzard-Patching

## Overview

When a player tries to login to the server, the server reads the client version and detects it is out of date and that there is a update available. The server sends a update to the client as part of the login protocol. The update contains a Blizzard Downloader executable. The Blizzard Downloader downloads a Blizzard Updater. The Blizzard Updater installs the MPQ updates and a new WoW executable with a updated build. Now when the player logs in they have the correct version and can play.

## Patching through the WoW client

See the PDF attached in this repository.

## The Blizzard Downloader

The Blizzard Downloader is responsible for downloading the Blizzard Updater. It supports HTTP direct downloads and the bittorrent protocol. We can send the entire update through the WoW client instead of using the Blizzard Downloader but this will stress the authserver and doing it that way loses the peer to peer aspect.

The Blizzard Downloader can be built with the resources under the following repository: https://github.com/stoneharry/Blizzard-Downloader

The downloader can be built using `Bwod.exe`, for example: `.\BwoD.exe compile torrent.torrent MyBackgroundDownloader.exe`. The program compiles the downloader using the `base.exe`, embedding the `skin.mpq`, and embedding the torrent data to use.

The `skin.mpq` file contains the images that are used to style the interface.

The torrent file is the file that the downloader should download and seed.

The torrent file can be created using any bittorrent program. Take note of the split size used, for me I set the split size to 16384 kb. Once you have created your torrent file you can use `BEncode Editor.exe` to edit the data in the torrent file. We can add various properties to the file that the Blizzard Downloader reads to know how to behave.

![Bencode screenshot](https://i.imgur.com/I6tO3Bw.png)

A full list of the possible parameters that I have reverse engineered:
```
download type
url
end
begin
server list
disable p2p
locale
notes url
direct download
choose download
show eula
done label
download label
autolaunch
launch target
creation date
announce
```

To support HTTP direct downloads we need to split the data for the torrent by the file size we defined. I set the size to 16384 kb, so I use the `filesplitter.exe` to split the installer by 16777216 bytes. I can then put this on my HTTP host and use the link for Direct HTTP downloads.

![Example HTTP Direct Download](https://i.imgur.com/vlT4V1V.png)

You need a tracker for the Blizzard Downloader to connect to and use to work out who is seeding and what direct downloads are available. I have tested using a C# library MonoTorrent and also using a very lightweight PHP framework: https://github.com/Hlkz/UpdateTracker

![Example Blizzard Downloader](https://i.imgur.com/z9POmpJ.png)

This works perfectly using HTTP direct downloads. However I have not managed to get the peer-to-peer aspect to work. When debugging my torrent tracker I can see that seeders and leachers are correctly registering with the tracker, so I am not sure the leachers do not start downloading data from the seeders.

When the Blizzard Downloader is run from the WoW directory it creates debug log output at the following directory: `C:\Users\Public\Documents\Blizzard Entertainment\World of Warcraft\Logs`

From my reverse engineering I believe it reads a `BlizzardDownloader.ini` from `C:\Program Files (x86)\Common Files\Blizzard Entertainment`.

Throughout the assembly I can see useful debug log statements made when a "test mode" is enabled. I have not managed to find a way to enable this mode.

Debug mode data:

![Reverse Engineer 1](https://i.imgur.com/ReUPTd5.png)
![Reverse Engineer 2](https://i.imgur.com/Cf1PqSb.png)
```
A4088h,1Ch,Logs/downloader-testmode.log
A40A8h,18h,Downloader Test Mode Log
A41A4h,8h,peer.log
A4B80h,14h,peer missing total: 
A4B98h,20h,bitfield received was too small!
A4BBCh,Dh,recv bitfield
A4BCCh,29h,peer sent a HAVE for a piece > numPieces!
A4BF8h,Ah,recv have 
A4C04h,1Ch,Invalid size for a have msg:
A4C24h,13h,recv not interested
A4C38h,Fh,recv interested
A4C48h,Ch,recv unchoke
A4C58h,Ah,recv choke
A4C64h,25h,dirty peer! peer sent bad data before
A4C8Ch,1Eh,already connected to this peer
A4CACh,8h,PeerID: 
A4CB8h,14h,processing handshake
A4CD0h,7h,", time="
A4CD8h,34h,Peer got too many bytes or too long duration: bytes=
A4D10h,1Dh,Peer sent too many bad pieces
A4D30h,22h,peer trying to send too much data:
```

We can also see where it appears to read in a bunch of parameters from somewhere:

![Reverse Engineer 3](https://i.imgur.com/d6OJSY1.png)
```
directDownloadURL
nop2p
nohttp
noseeds
onlyblizzpeers
noblizzpeers
trackerless
allowincoming
testmode
```

Considering the Direct Downloader URL is sent from the tracker, I wonder if the other parameters need to be supplied that way somehow:
```php
		// begin response
		$response = 'd8:intervali' . $_SERVER['tracker']['announce_interval'] . 
		$response = strlen($direct_download[0]) ? 'd6:directd3:url' . strlen($direct_download[0]) . ':' . $direct_download[0] . '9:thresholdi10000000ee' : 'd';
		$response .= '8:intervali' . $_SERVER['tracker']['announce_interval'] . 
		            'e12:min intervali' . $_SERVER['tracker']['min_interval'] . 
		            'e5:peers';

```

I have tested trying to set test mode through various properties in the torrent file and have also tried setting it in the `BlizzardDownloader.ini` file it tries to read. Another thing I have noted is that in the logs we can see it trying to connect two hardcoded IP's for the ini file:

`17:55:45.0571 Checking for server side config http://206.16.22.130/update/Downloader.ini`

`15:57:40.6400 Checking for server side config http://12.129.232.131/update/Downloader.ini`

I could not find in the binary where it is defining this IP to try to connect to. It appears to use one of the two each run at random, and doesn't try connecting to both in a single run.

From old IRC logs I can see http://us.version.worldofwarcraft.com/update/PatchSequenceFile.txt mapped to http://206.16.22.126/update/PatchSequenceFile.txt

I did test trying to run the Blizzard Downloader with debug arguments to try and trigger debug mode, but with no success. I think the only arguments are: `--hiddenProcessing` which runs the program headless, and `--hiddenPort=%d`.

### 2021-06-29

I did manage to get debug mode enabled with Schlumpf's help. It looks like all usage has been stripped from the code however, including parsing the argument from the command line arguments. We replaced `74 19 C6 46 68 01 C6 86 DB 00` with `90 90 C6 46 68 01 C6 86 DB 00` to force debug mode to be enabled. 

To enable test mode:
```
let .text:00465BEB                 jz      short loc_465C06 be a nop (0x90).
```
Search for `74 19 C6 46 68 01 C6 86 DB 00` in the binary, replace with `90 90 …`.

`.\HourOfTwilight_Downloader.exe --noblizzpeers --port=3725`
Doesn't appear to output anything differently
`.\HourOfTwilight_Downloader.exe --port=3725 --noblizzpeers -log log -noblizpeers noblizpeers port 3725 -port 3725 -onlyblizzpeers false onlyblizzpeers false`
Something in this set of parameters causes it to log in a folder called XXogs\downloader-testmode.log where XX is random characters each run

![WoW Folder with bad Logs folder](https://i.imgur.com/lHE7GFi.png)

This logs to `downloader-testmode.log`:
```
#-----------------------------------------------------------
# System started at 2021-04-19 20:32:53.1403
# system: HARRY
#-----------------------------------------------------------
20:32:56.4410 Downloader Test Mode Log
20:36:11.4058 Done
```

We also discovered that the IP's it connects to is: one of `(us|eu|kr|cn|tw).version.worldofwarcraft.com`. We can disable it pinging this IP by changing:
```
.text:00402211 6A 01                                   push    1 be push 0.
```

Full list of possible arguments:
```
version
seed
flood
port
choosepath
responsefile
responselist
locale
url
tracker
checkhashes
minuploadrate
maxuploadrate
launchtarget
maxsimultaneous
maxuploads
maxallowin
allowincoming
trackerless
spew
passThroughParams
updateinterval
saveas
minpeers
noblizseeds
onlyblizseeds <<<<< 
noblizpeers
onlyblizpeers
noseeds
piecestorage
nohttp
nop2p <<<<<<<
background
bgseed
autolaunch
maxpending
cookieName
cookieData
directDownloadURL
logfile <<<<
log <<< 
logFolder <<<<
silent
hiddenProcessing
hiddenPort
closewhendiskfull
autothrottle
usemetafile
scratch
rescanifbadpiece
memcached
mcconfpath
idleopt
maxallowwhenslow
upnpby
dontusetracker
dontusebonjour
streaming
mtui
downloadlabel
usenewthreshold
uponly
noskin
help
usage
```

I still have no clue why the peer to peer aspect of the Downloader is not working.

## The Blizzard Updater

The main Blizzard Updater to use is documented here: https://github.com/stoneharry/Blizzard-Updater

It builds an incremental update targeting an existing installation.

If you need to do a major update like a full expansion, or want to install the client from scratch, then the full client installer has a lot more functionality but is a lot more heavyweight: https://github.com/stoneharry/Blizzard-Client-Installer

## Changing the WoW client version

The client build is stored as a uint16 in a few places in the binary. The one that is read by the authserver during authentication in 3.3.5 is at: `0x4C99F0`.

A very simple program to change the WoW version:
```csharp
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WoWVersionEditor
{
    class Program
    {
        private const uint _VersionOffset = 0x4C99F0; // 5020144 

        static void Main(string[] args)
        {
            try
            {
                var path = args[0];
                var currentVersion = ReadVersion(path);
                var newVersion = args[1].Equals("+") ? ushort.Parse((currentVersion + 1).ToString()) : ushort.Parse(args[1]);
                Console.WriteLine("Updating: " + path);
                Console.WriteLine("Current version: " + currentVersion);
                Console.WriteLine("New version: " + newVersion);
                WriteVersion(path, $"{path}_{newVersion}.exe", newVersion);
                Console.WriteLine("Done");
            }
            catch (Exception e)
            {
                Console.WriteLine($"[ERROR] {e.GetType()}: {e.Message}\n{e}");
            }
        }

        static ushort ReadVersion(string filePath)
        {
            using (var stream = File.Open(filePath, FileMode.Open))
            {
                stream.Position = _VersionOffset;
                using (var reader = new BinaryReader(stream))
                {
                    return reader.ReadUInt16();
                }
            }
        }

        static void WriteVersion(string oldFilePath, string newFilePath, ushort version)
        {
            File.Copy(oldFilePath, newFilePath, true);
            using (var stream = File.Open(newFilePath, FileMode.Open))
            {
                stream.Position = _VersionOffset;
                using (var reader = new BinaryWriter(stream))
                {
                    reader.Write(version);
                }
            }
        }
    }
}
```
