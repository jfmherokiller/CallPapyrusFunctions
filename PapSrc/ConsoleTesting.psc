Scriptname consoletesting extends ReferenceAlias

Event OnInit()
	UnregisterForMenu("Console")
	RegisterForMenu("Console")
endEvent

Event OnPlayerLoadGame()
	UnregisterForMenu("Console")
	RegisterForMenu("Console")
endEvent
 
Event OnMenuOpen(string menuName)
	if menuName=="Console"
		RegisterForKey(28)
		RegisterForKey(156)
	endif
endEvent
 
Event OnMenuClose(string menuName)
	if menuName=="Console"
		UnregisterForKey(28)
		UnregisterForKey(156)
	endif
endEvent

Bool Function ChoseCommandTable(string[] cmd)
	; code
	actor a = Game.GetCurrentConsoleRef() as actor
	if a==none
		a=Game.GetPlayer()
	endif
	bool bSuccess=false
	if cmd[0]=="version"
		Debug.Notification("This is version 1.0!")
		bSuccess=true
	ElseIf(cmd[0] == "jseval")
		MyPluginScript.EvalateJsExpression(cmd[1])
	ElseIf(cmd[0] == "jseval2")
		MyPluginScript.EvalateJsExpression("std.evalScript("+cmd[1]+")")
	ElseIf(cmd[0] == "cgfm")
		MyPluginScript.CallGlobalFunction(cmd[1],cmd[2])
	endif
	return bSuccess
	
EndFunction
Event OnKeyDown(int keyCode)
	if keyCode==28 || keyCode==156
		int cmdCount = UI.GetInt("Console", "_global.Console.ConsoleInstance.Commands.length")
		if cmdCount>0
			cmdCount-=1
			string cmdLine = UI.GetString("Console","_global.Console.ConsoleInstance.Commands."+cmdCount)
			if cmdLine!=""
				bool bSuccess=false
				string[] cmd=StringUtil.Split(cmdLine," ")
				bSuccess = ChoseCommandTable(cmd)
				if bSuccess==true
					; Remove last line (error line)
					Utility.WaitMenuMode(0.1)
					string history = UI.GetString("Console","_global.Console.ConsoleInstance.CommandHistory.text")
					int iHistory = StringUtil.GetLength(history) - 1
					bool bRunning=true
					while iHistory>0 && bRunning==true
						if StringUtil.AsOrd(StringUtil.GetNthChar(history,iHistory - 1))==13
							bRunning=false
						else
							iHistory-=1
						endif
					endWhile
					UI.SetString("Console","_global.Console.ConsoleInstance.CommandHistory.text",StringUtil.Substring(history,0,iHistory))
				endif
			endif
		endif
	endif
endEvent