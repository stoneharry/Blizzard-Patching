void CGlueMgr::PatchDownloadApply()
{
  int reason_for_failure = 5;

  char old_cwd[PATH_MAX];
  OsGetCurrentDirectory (sizeof (old_cwd), old_cwd);
  OsSetCurrentDirectory (OsFileGetDownloadFolder());

  m_deleteLocalPatch = false;

  Blizzard::Mopaq::HSARCHIVE__* archive = NULL;

  if (SFileOpenArchive ("wow-patch.mpq", 100, 0, &archive))
  {
    Blizzard::Mopaq::AuthResult authresult;
    SFileAuthenticateArchiveEx ( archive, &authresult
                               , &modulus, sizeof(modulus)
                               , &exponent, sizeof(exponent)
                               );

    if (authresult > 4)
    {
      if (PatchDownloadExecutePrepatch (archive))
      {
        SFileCloseArchive (archive);
        archive = NULL;

        if (m_deleteLocalPatch)
          OsDeleteFile ("wow-patch.mpq");

        OsSetCurrentDirectory (old_cwd);

        QuitGame();
        return;
      }
      else
      {
        reason_for_failure = 6;
      }
    }

    SFileCloseArchive (archive);
    archive = NULL;
  }
  else if (SErrGetLastError() == 2)
  {
    reason_for_failure = 4;
  }

  PatchFailed (reason_for_failure, 0);
  OsDeleteFile ("wow-patch.mpq");

  OsSetCurrentDirectory (old_cwd);
}
