using System.Globalization;

namespace ESIR.Web.Components.Shared;

public partial class BreathingExercise : IDisposable
{
    private sealed record Pattern(string Name, string Summary, int In, int HoldIn, int Out, int HoldOut);

    private static readonly Pattern[] Patterns =
    [
        new("Slow and calm", "In 4, hold 2, out 6", 4, 2, 6, 0),
        new("Box breathing", "In 4, hold 4, out 4, hold 4", 4, 4, 4, 4),
        new("Long breath out", "In 4, out 8", 4, 0, 8, 0),
    ];

    private static readonly int[] RoundChoices = [3, 5, 8];

    private Pattern pattern = Patterns[0];
    private int totalRounds = 3;

    private CancellationTokenSource? cts;
    private bool running;
    private bool finished;
    private int round;
    private int remaining;
    private double scale = 1;
    private int seconds = 1;
    private string instruction = "Ready when you are.";
    private string faceMood = "neutral";

    // Always use a dot as decimal separator, because CSS does not accept a comma.
    private string Scale => scale.ToString(CultureInfo.InvariantCulture);

    private void Choose(Pattern p)
    {
        if (running) return;
        pattern = p;
        finished = false;
        instruction = "Ready when you are.";
    }

    private void ChooseRounds(int n)
    {
        if (!running) totalRounds = n;
    }

    private async Task Start()
    {
        cts?.Cancel();
        cts = new CancellationTokenSource();
        var ct = cts.Token;
        running = true;
        finished = false;
        faceMood = "calm";

        try
        {
            for (round = 1; round <= totalRounds; round++)
            {
                await Phase("Breathe in…", 1.5, pattern.In, ct);
                if (pattern.HoldIn > 0) await Phase("Hold…", 1.5, pattern.HoldIn, ct);
                await Phase("Breathe out slowly…", 1.0, pattern.Out, ct);
                if (pattern.HoldOut > 0) await Phase("Rest…", 1.0, pattern.HoldOut, ct);
            }

            finished = true;
            faceMood = "happy";
            instruction = "Well done. Notice how you feel now.";
        }
        catch (OperationCanceledException)
        {
            instruction = "Stopped. Start again whenever you like.";
            faceMood = "neutral";
            scale = 1;
            seconds = 1;
        }
        finally
        {
            running = false;
            remaining = 0;
            await InvokeAsync(StateHasChanged);
        }
    }

    private async Task Phase(string text, double toScale, int duration, CancellationToken ct)
    {
        instruction = text;
        scale = toScale;
        seconds = duration;
        for (remaining = duration; remaining > 0; remaining--)
        {
            await InvokeAsync(StateHasChanged);
            await Task.Delay(1000, ct);
        }
    }

    private void Stop() => cts?.Cancel();

    public void Dispose()
    {
        cts?.Cancel();
        cts?.Dispose();
    }
}