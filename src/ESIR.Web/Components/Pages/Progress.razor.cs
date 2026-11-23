namespace ESIR.Web.Components.Pages;

public partial class Progress
{
    private sealed record PlanStep(string Title, string Text);
    private sealed record Part(string Title, string Detail, string Status);
    private sealed record LayerRow(string Name, string Needs);

    private readonly PlanStep[] _plan =
    [
        new("Agree.", "Decide the controls, the states and the body, and print a fit test."),
        new("Put it together.", "Join the firmware, the body and the public website."),
        new("Finish.", "In the last two or three days we test, write things down and rehearse."),
    ];

    // Update a status here only when it is true.
    private readonly Part[] _must =
    [
        new("Touch, tilt, shake and pick-up", "Touch pads (tap and hold), tilt and shake with a cooldown, and an estimate of when ESIR is lifted.", "Planned"),
        new("Faces, lights, sleep and wake", "An animated face and RGB lights. ESIR falls asleep when left alone and wakes up again.", "Planned"),
        new("Six feelings on the website", "Happy, sad, stressed, angry, tired and bored, each with a matching answer on the robot and on the website.", "In progress"),
        new("Breathing, hearts and messages", "A guided breathing animation, hearts and kind messages.", "In progress"),
        new("Pause, stop and safe limits", "Stop always wins. Limits on lights and sound, and nothing switched on after a restart.", "Planned"),
        new("Wi-Fi link and confirmed status", "The robot asks for commands over Wi-Fi and reports back. The website shows only what is confirmed.", "In progress"),
        new("The website", "Pages about ESIR, the team and our progress, plus the feelings and the exercises.", "In progress"),
        new("A fixed printed case", "A flat disc shell, designed in CAD, with a display window, open touch pads and USB access.", "Planned"),
    ];

    private readonly LayerRow[] _layers =
    [
        new("Firmware and states", "Arduino C++ on the MKR WiFi 1010 with a loop that never waits. The states are idle, responding, breathing, paused and sleeping."),
        new("Input and output", "Touch with debouncing (tap and hold). Tilt and shake from the motion sensor, with a cooldown, and an estimate of pick-up. A face and RGB lights."),
        new("Safety and power", "Output limits with a way to stop and disable. Steady USB power and a safe low-power mode. No lights or sounds after a restart."),
        new("Connection", "HTTP and JSON over Wi-Fi: the robot asks for commands and posts its status. Short timeouts, and command IDs so repeats are skipped."),
        new("Website", "Blazor Server on ASP.NET Core, with one shared command service for each robot."),
        new("Body and tools", "A fixed 3D-printed case designed in CAD. PlatformIO for the firmware and a shared task board for planning."),
    ];

    private readonly string[] _done_when =
    [
        "Every touch and motion input gives the agreed response.",
        "All six feelings give matching feedback on the robot and on the website.",
        "The website shows confirmed progress and the offline state.",
        "The offline controls work, and old commands are ignored.",
        "The case fits, the power is steady, and repeated tests need no reset.",
    ];

    private readonly string[] _v2 =
    [
        "Finer gestures and smoother changes between faces",
        "Eye colour and brightness you can choose",
        "Recovery when something goes wrong, and clearer feedback",
        "Testing both versions of the Opla carrier",
        "A simple activity with a sequence of touches",
        "Extra feelings, if there is time",
    ];

    private readonly string[] _v3 =
    [
        "A second body with one moving head axis",
        "A microphone, only after privacy checks",
        "Voice replies through a laptop",
        "More sensors, gestures and games",
        "ESIRs you can customise",
    ];

    private int _total => _must.Length;
    private int _done => Count("Done");
    private int _inProgress => Count("In progress");
    private int _planned => Count("Planned");

    private static string BadgeClass(string status) => status switch
    {
        "Done" => "badge--done",
        "In progress" => "badge--inprogress",
        _ => ""
    };

    private int Count(string status) => _must.Count(p => p.Status == status);
}