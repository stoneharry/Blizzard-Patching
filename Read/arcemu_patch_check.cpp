if(build < LogonServer::getSingleton().min_build)
{
  char flippedloc[5] = {0,0,0,0,0};
  flippedloc[0] = m_challenge.country[3];
  flippedloc[1] = m_challenge.country[2];
  flippedloc[2] = m_challenge.country[1];
  flippedloc[3] = m_challenge.country[0];

  m_patch = PatchMgr::getSingleton().FindPatchForClient(build, flippedloc);
  if(m_patch == NULL)
  {
    LOG_DETAIL("[AuthChallenge] Client %s has wrong version. Server: %u, Client: %u", GetRemoteIP().c_str(), LogonServer::getSingleton().min_build, m_challenge.build);
    SendChallengeError(CE_WRONG_BUILD_NUMBER);
    return;
  }
}
