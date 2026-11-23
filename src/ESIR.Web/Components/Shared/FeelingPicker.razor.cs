using ESIR.Web.Services;
using Microsoft.AspNetCore.Components;

namespace ESIR.Web.Components.Shared;

/// <summary>
/// Lets the visitor pick a feeling. ESIR answers on the screen and the feeling is queued for the robot.
/// Also shows whether the robot is online.
/// </summary>
public partial class FeelingPicker : IDisposable
{
    [Inject] private RobotStore Store { get; set; } = default!;

    private RobotSnapshot snapshot = new(false, null, null);
    private Timer? refresh;

    private string CurrentMood => snapshot.LastCommand?.Emotion ?? "neutral";
    private string Message => Messages.GetValueOrDefault(CurrentMood, Messages["neutral"]);

    protected override void OnInitialized()
    {
        snapshot = Store.GetSnapshot();
        Store.Changed += OnStoreChanged;
        // Check again every 5 seconds, so "offline" shows when the robot stops asking for commands
        refresh = new Timer(_ => OnStoreChanged(), null, TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(5));
    }

    private void Send(string emotion)
    {
        Store.Enqueue(emotion);
        snapshot = Store.GetSnapshot();
    }

    private void OnStoreChanged() => _ = InvokeAsync(() =>
    {
        snapshot = Store.GetSnapshot();
        StateHasChanged();
    });

    private static readonly Dictionary<string, string> Messages = new()
    {
        ["neutral"] = "Hello! I'm ESIR.",
        ["happy"] = "I'm happy to see you!",
        ["sad"] = "I'm here with you.",
        ["stressed"] = "Let's breathe together. In... and out.",
        ["angry"] = "It's okay to feel this. Want to take a pause?",
        ["tired"] = "A short break might help. I'll wait with you.",
        ["bored"] = "Let's find something fun to do!",
    };

    private static string Label(string emotion) => char.ToUpperInvariant(emotion[0]) + emotion[1..];

    private static string StatusText(CommandStatus status) => status switch
    {
        CommandStatus.Pending => "waiting for ESIR",
        CommandStatus.Accepted => "ESIR got it",
        CommandStatus.Running => "ESIR is responding",
        CommandStatus.Completed => "done",
        CommandStatus.Expired => "ESIR did not answer",
        _ => status.ToString()
    };

    public void Dispose()
    {
        Store.Changed -= OnStoreChanged;
        refresh?.Dispose();
    }
}