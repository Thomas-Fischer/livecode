﻿script "TestRunnerBehavior"
/*
Copyright (C) 2016 LiveCode Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

-- FIXME provide this on the command line
constant kLogFilename = "_test_suite.log"

----------------------------------------------------------------
-- Top-level actions
----------------------------------------------------------------

function getCommandLineInfo
   local tRawArg, tSelfCommand, tSelfScript, tInArgs, tArgs

   put false into tInArgs

   -- Treat everything up to & including the first
   -- ".livecodescript" file as the command for running the test
   -- runner, and everything after it as test runner arguments
   put the commandName into tSelfCommand[1]
   repeat for each element tRawArg in the commandArguments

      if tInArgs then
         put tRawArg into tArgs[1 + the number of elements in tArgs]
      else
         put tRawArg into tSelfCommand[1 + the number of elements in tSelfCommand]
         if tRawArg ends with ".livecodescript" then
            put tRawArg into tSelfScript
            put true into tInArgs
         end if
      end if

   end repeat

   local tInfo
   put tSelfCommand into tInfo["self-command"]
   put tSelfScript into tInfo["self-script"]
   put tArgs into tInfo["args"]

   return tInfo
end getCommandLineInfo

command TestRunnerMain
   local tInfo
   put getCommandLineInfo() into tInfo

   switch tInfo["args"][1]
      case "invoke"
         doInvoke tInfo
         break
      case "run"
         doRun tInfo
         break
      case "--help"
      case "-h"
      case "help"
         doUsage 0
         break
      default
         doUsage 1
         break
   end switch
   quit 0
end TestRunnerMain

private command doInvoke pInfo
   put pInfo["args"][2] into pInfo["invoke"]["script"]
   put pInfo["args"][3] into pInfo["invoke"]["command"]
   
   dispatch "TestRunnerImpl_DoInvoke" to me with pInfo
end doInvoke

private command doRun pInfo
   if pInfo["args"][2] is empty then
      doUsage 1
   end if
   
   TestRunnerRunLoadLibraries

   local tAnalysis
   dispatch "TestRunnerImpl_DoRun" to me with pInfo
   put the result into tAnalysis
   
   -- Save the log to file.
   -- We process to binary data ourselves to ensure encoding and
   -- line endings are appropriate.
   local tLogForWriting
   put textEncode(tAnalysis["log"], "utf8") into tLogForWriting
   if the platform is "win32" then
      replace return with numToChar(13) & numToChar(10) in tLogForWriting
   end if
   put tLogForWriting into url ("binfile:" & kLogFilename)
   
   if TesterTapGetWorstResult(tAnalysis) is "FAIL" then
      quit 1
   end if
end doRun

private command doUsage pStatus
   dispatch "TestRunnerImpl_DoUsage" to me with pStatus
end doUsage

on ErrorDialog pExecutionError
   write "ERROR:" && pExecutionError & return to stderr
   quit 1
end ErrorDialog

private function getStackFolder
   local tFilename
   put the effective filename of this me into tFilename
   
   set the itemDelimiter to slash
   return item 1 to -2 of tFilename
end getStackFolder

private command TestRunnerLoadLibraries pLibraries
   local tLibrary, tStackName, tStackFile
   repeat for each item tLibrary in pLibraries
      -- Compute the filename of the library stack and load it
      put getStackFolder() & slash & tLibrary & ".livecodescript" into tStackFile
      put the name of stack tStackFile into tStackName
      
      send "revLoadLibrary" to stack tStackname
   end repeat
end TestRunnerLoadLibraries

----------------------------------------------------------------
-- Support for invoking test commands
----------------------------------------------------------------

-- Add the unit test library stack and the input library to the backscripts
constant kInvokeLibraries = "_testlib,_inputlib"
command TestRunnerInvokeLoadLibraries
   TestRunnerLoadLibraries kInvokeLibraries
end TestRunnerInvokeLoadLibraries

-- Execute a test
command TestRunnerInvokeTestCommand pScriptFile, pCommand
   local tStackName, tResult
   -- This should auto-load the test script
   put the name of stack pScriptFile into tStackName
   
   -- Check that the stack script actually compiles
   lock messages
   set the script of tStackname to the script of tStackname
   put the result into tResult
   unlock messages
   if tResult is not empty then
      TestDiagnostic tResult
   end if
   TestAssert "script compiles", tResult is empty
   
   -- Don't run test if script doesn't compile
   if tResult is not empty then
      exit TestRunnerInvokeTestCommand
   end if
   
   try
      dispatch "TestSetup" to tStackName
      put the result into tResult
   catch tError
      TestAssert "setup test", false
      TestDiagnostic tError   
   end try   
   
   if word 1 of tResult is "SKIP" then
      TestSkip pCommand, word 2 to -1 of tResult
   else
      -- Run the actual test itself
      try
         dispatch pCommand to tStackName
      catch tError      
         TestAssert "execute test", false
         TestDiagnostic tError   
         -- don't exit here as test cleanup may still be useful
      end try
   end if
   
   -- Do common cleanup tasks
   try
      dispatch "TestTearDown" to tStackName
   catch tError
      TestAssert "tear down test", false
      TestDiagnostic tError  
   end try
end TestRunnerInvokeTestCommand

----------------------------------------------------------------
-- Support for running tests
----------------------------------------------------------------

-- Add the test runner library stack to the backscripts
constant kRunLibraries = "_testerlib"
command TestRunnerRunLoadLibraries
   TestRunnerLoadLibraries kRunLibraries
end TestRunnerRunLoadLibraries

command TestRunnerProcessOutput pScriptfile, pCommand, pOutput
   -- Create test log
   local tTestLog
   put "###" && TesterGetPrettyTestName(pScriptFile) && pCommand \
         & return & return into tTestLog
   put pOutput & return after tTestLog
   
   -- Analyse the results and print a summary line
   local tTapResults
   put TesterTapAnalyse(tTestLog) into tTapResults

   TesterLogSummaryLine tTapResults, (TesterGetPrettyTestName(pScriptFile) & ":" && pCommand)

   return tTapResults
end TestRunnerProcessOutput

-- Print out a table of statistics
command TestRunnerRunPrintSummary pAnalysis
   local tSummaryString, tTotal, tDecoration

   put TesterTapGetTestCount(pAnalysis) into tTotal

   -- Format basic summary information
   if pAnalysis["xfail"] is 0 and pAnalysis["fail"] is 0 then
      put "All" && tTotal && "tests passed" into tSummaryString

   else if pAnalysis["fail"] is 0 then
      put "All" && tTotal && "tests behaved as expected" into tSummaryString

   else
      put pAnalysis["fail"] && "OF" && tTotal && "TESTS FAILED" into tSummaryString
   end if

   put return after tSummaryString
   
   -- Add extra summary info from expected failure & skip directives
   if pAnalysis["xpass"] > 0 then
      put tab & pAnalysis["xpass"] && "unexpected passes" & return after tSummaryString
   end if
   if pAnalysis["xfail"] > 0 then
      put tab & pAnalysis["xfail"] && "expected failures" & return after tSummaryString
   end if
   if pAnalysis["skip"] > 0 then
      put tab & pAnalysis["skip"] && "skipped" & return after tSummaryString
   end if

   put "================================================================" into tDecoration
   put tDecoration & return before tSummaryString
   put tDecoration & return after tSummaryString

   write tSummaryString to stdout
end TestRunnerRunPrintSummary
