const int cycles_per_second (1000);

while(mrunning.GetVal())
{
  if(!(++loop_counter % (20 * cycles_per_second)))  // 20 seconds
    CheckForDeadSockets();

  if(!(loop_counter % (300 * cycles_per_second)))	  // 5 mins
    ThreadPool.IntegrityCheck();

  if(!(loop_counter % (5 * cycles_per_second)))
  {
    sInfoCore.TimeoutSockets();
    sSocketGarbageCollector.Update();
    CheckForDeadSockets();			                    // Flood Protection
    UNIXTIME = time(NULL);
    g_localTime = *localtime(&UNIXTIME);
  }

  PatchMgr::getSingleton().UpdateJobs();
  Arcemu::Sleep(1000 / cycles_per_second);
}
