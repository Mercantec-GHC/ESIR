namespace ESIR.Web.Components.Pages;

public partial class AboutUs
{
    private sealed record Step(string Title, string Text, string Mood);

    private readonly Step[] _steps =
    [
        new("Tell ESIR how you feel", "Pick a feeling: happy, sad, stressed, angry, tired or bored.", "happy"),
        new("ESIR answers", "You see ESIR's face change and read a kind message. The robot will also show it with light.", "calm"),
        new("Slow down together", "When you want to, try a breathing exercise or another short exercise.", "sleepy"),
    ];
}