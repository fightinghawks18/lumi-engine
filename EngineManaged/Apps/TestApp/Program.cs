using Lumi.Sys;

Console.WriteLine("Hello, World!");

var windowManager = new WindowManager();
var window = windowManager.CreateWindow();

while (true)
{
    if (window.Closing()) break;
    windowManager.Update();
}

 windowManager.Delete();