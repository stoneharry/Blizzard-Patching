while(mrunning.GetVal())
{
  if(!(++loop_counter % 20))	            // 20 seconds
    CheckForDeadSockets();

  if(!(loop_counter % 300))	              // 5 mins
    ThreadPool.IntegrityCheck();

  if(!(loop_counter % 5))
  {
    sInfoCore.TimeoutSockets();
    sSocketGarbageCollector.Update();
    CheckForDeadSockets();			          // Flood Protection
    UNIXTIME = time(NULL);
    g_localTime = *localtime(&UNIXTIME);
  }

  PatchMgr::getSingleton().UpdateJobs();
  Arcemu::Sleep(1000);
}
