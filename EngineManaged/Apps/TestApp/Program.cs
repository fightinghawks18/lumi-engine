using Lumi.Sys;

using IniParser;
using IniParser.Model;

string dir = AppContext.BaseDirectory;
string parent = Path.Combine(dir, "../");
string iniPath = Path.Combine(parent, "Assets", "window.ini");

var parser = new FileIniDataParser();
IniData data = parser.ReadFile(iniPath);

var windowManager = new WindowManager();

var winProps = new WindowProperties
{
    title = data["Window"]["title"].Trim('"'),
    icon = Path.Combine(parent, data["Window"]["icon"].Trim('"')),
    x = Window.GetWindowPosCentered(),
    y = Window.GetWindowPosCentered(),
    w = int.Parse(data["Window"]["width"]),
    h = int.Parse(data["Window"]["height"]),
    wMin = 800,
    wMax = 1200,
    hMin = 600,
    hMax = 1200,
    mode = (WindowMode)int.Parse(data["Window"]["mode"]),
    resizable = bool.Parse(data["Window"]["resizable"]),
    bordered = bool.Parse(data["Window"]["bordered"])
};

var win = windowManager.CreateWindow(winProps);

while (true)
{
    if (win.Closing()) break;
    windowManager.Update();
}

 windowManager.Delete();