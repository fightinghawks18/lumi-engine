using Lumi.Sys;

Console.WriteLine("Hello, World!");

var windowManager = new WindowManager();

var winProps = new WindowProperties
{
    title = "Test Window",
    x = 1200,
    y = 300,
    w = 800,
    h = 600
};

var win = windowManager.CreateWindow(winProps);

while (true)
{
    if (win.Closing()) break;
    windowManager.Update();
}

 windowManager.Delete();