using Lumi.Sys;

Console.WriteLine("Hello, World!");

var windowManager = new WindowManager();
var window = windowManager.CreateWindow();

Thread.Sleep(1000);

windowManager.Delete();