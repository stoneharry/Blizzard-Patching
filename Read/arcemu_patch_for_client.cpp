void PatchMgr::InitializePatchList()
{
  Log.Notice ("PatchMgr", "Loading Patches...");

  const size_t path_length (MAX_PATH * 10);

  char base_path[path_length];
  char absolute_filename[path_length];

#ifdef WIN32
  char file_pattern[path_length];

  if (!GetCurrentDirectory (sizeof (file_pattern), file_pattern))
    return;

  strcpy (base_path, file_pattern);
  strcat (file_pattern, "\\ClientPatches\\*.*");

  WIN32_FIND_DATA fd;
  HANDLE fHandle (FindFirstFile (file_pattern, &fd));
  if (fHandle == INVALID_HANDLE_VALUE)
    return;
#else
  strcpy (base_path, ".");

  struct dirent ** list (NULL);
  int filecount (scandir ("./ClientPatches", &list, 0, 0));
  if (filecount <= 0 || list== NULL)
  {
    Log.Error("PatchMgr", "No patches found.");
    return;
  }
#endif

#ifdef WIN32
  do
#else
  while (filecount--)
#endif
  {
#ifdef WIN32
    snprintf (absolute_filename, sizeof (absolute_filename), "%s\\ClientPatches\\%s", base_path, fd.cFileName);
#else
    snprintf (absolute_filename, sizeof (absolute_filename), "%s/ClientPatches/%s", base_path, list[filecount]->d_name);
#endif

    uint32 srcversion;
    char locale[5] = "****";
    if (sscanf(fd.cFileName,"%4s%u.", locale, &srcversion) != 2)
    {
      Log.Notice ("PatchMgr", "Found incorrect patch file: %4s %s", locale, fd.cFileName);
      continue;
    }

#ifdef WIN32
    HANDLE hFile (CreateFile (absolute_filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL));
    if(hFile == INVALID_HANDLE_VALUE)
      continue;
#else
    const int file_descriptor (open (absolute_filename, O_RDONLY));

    if (file_descriptor <= 0)
    {
      LOG_ERROR("Cannot open %s", absolute_filename);
      continue;
    }

    struct stat stat_buffer;
    if(fstat(file_descriptor, &stat_buffer) < 0)
    {
      LOG_ERROR("Cannot stat %s", absolute_filename);
      continue;
    }
#endif

    Log.Notice ("PatchMgr", "Found patch for %u locale `%s`.", srcversion, locale);

    Patch* patch (new Patch);

#ifdef WIN32
    DWORD sizehigh;
    DWORD size (GetFileSize (hFile, &sizehigh));
#else
    unsigned int size (stat_buffer.st_size);
#endif
    patch->FileSize = size;
    patch->Data = new uint8[size];
    patch->Version = srcversion;
    for(size_t i (0); i < 4; ++i)
      patch->Locality[i] = static_cast<char> (tolower (patch->Locality[i]));
    patch->Locality[4] = '\0';
    patch->uLocality = *(uint32*) (patch->Locality);

#ifdef WIN32
    const bool result (ReadFile (hFile, patch->Data, patch->FileSize, &size, NULL));
#else
    size = read (file_descriptor, pPatch->Data, size);
    const bool result (size > 0);
#endif
    ASSERT (result);
    ASSERT (size == patch->FileSize);

#ifdef WIN32
    CloseHandle (hFile);
#else
    close (file_descriptor);
#endif

    MD5Hash md5;
    md5.Initialize();
    md5.UpdateData (patch->Data, patch->FileSize);
    md5.Finalize();
    memcpy (patch->MD5, md5.GetDigest(), MD5_DIGEST_LENGTH);

    m_patches.push_back (patch);

#ifndef WIN32
    free (list[filecount]);
#endif
  }
#ifdef WIN32
  while (FindNextFile (fHandle, &fd));

  FindClose(fHandle);
#else
  free (list);
#endif
}

const Patch* PatchMgr::FindPatchForClient(uint32 Version, const char * locale) const
{
  const char lower_case[4] = {tolower (locale[0]), tolower (locale[1]), tolower (locale[2]), tolower (locale[3])};
  const uint32 ulocale (*(uint32*)lower_case);

  const Patch * fallbackPatch (NULL);
  for( std::vector<Patch*>::const_iterator patch_it (m_patches.begin())
     ; patch_it != m_patches.end()
     ; ++patch_it
     )
  {
    const Patch* patch (*patch_it);
    if(patch->uLocality == ulocale)
    {
      if(patch->Version == Version)
        return patch;

      if(fallbackPatch == NULL && patch->Version == 0)
        fallbackPatch = patch;
    }
  }

  return fallbackPatch;
}
