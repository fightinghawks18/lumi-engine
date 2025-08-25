using Lumi.Sys;

string dir = AppContext.BaseDirectory;
string iconPath = Path.Combine(dir, "../../assets", "icon.bmp");

var windowManager = new WindowManager();

var winProps = new WindowProperties
{
    title = "Test Window",
    icon = iconPath,
    x = Window.GetWindowPosCentered(),
    y = Window.GetWindowPosCentered(),
    w = 800,
    h = 600,
    wMin = 800,
    wMax = 1200,
    hMin = 600,
    hMax = 1200,
    mode = WindowMode.Windowed,
    resizable = true,
    bordered = true
};

var win = windowManager.CreateWindow(winProps);

while (true)
{
    if (win.Closing()) break;
    windowManager.Update();
}

 windowManager.Delete();