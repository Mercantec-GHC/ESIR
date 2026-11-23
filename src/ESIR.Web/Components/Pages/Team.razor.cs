namespace ESIR.Web.Components.Pages;

public partial class Team
{
    private sealed record Member(string Name, string Role, string Work);

    private readonly Member[] _members =
    [
        new("Yuliia", "Website",
            "Works mostly on the website, and takes some tasks on the backend (the server)."),
        new("Victoria", "Backend and the Arduino connection",
            "Builds the backend and the connection between the website and the Arduino."),
        new("Patricia", "Arduino, design and 3D model",
            "Works on the Arduino program, the design of the robot and the 3D model that we print."),
    ];
}