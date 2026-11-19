using ESIR.Web.Services;
using Microsoft.AspNetCore.Components;

namespace ESIR.Web.Components.Pages;

public partial class Feelings : IDisposable
{
    [Inject] private RobotStore Store { get; set; } = default!;

    private RobotCommand? lastCommand;

    protected override void OnInitialized()
    {
        lastCommand = Store.GetSnapshot().LastCommand;
        Store.Changed += OnStoreChanged;
    }

    private void Send(string emotion) => lastCommand = Store.Enqueue(emotion);

    private void OnStoreChanged() => _ = InvokeAsync(() =>
    {
        lastCommand = Store.GetSnapshot().LastCommand;
        StateHasChanged();
    });

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

    public void Dispose() => Store.Changed -= OnStoreChanged;
}
