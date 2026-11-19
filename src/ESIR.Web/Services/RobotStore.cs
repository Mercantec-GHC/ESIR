namespace ESIR.Web.Services;

public enum CommandStatus { Pending, Accepted, Running, Completed, Expired }

public sealed record RobotCommand(
    Guid Id,
    string Emotion,
    DateTimeOffset CreatedAt,
    DateTimeOffset ExpiresAt,
    CommandStatus Status);

public sealed record RobotSnapshot(
    bool Connected,
    DateTimeOffset? LastSeen,
    RobotCommand? LastCommand);

/// <summary>
/// In-memory state shared by the website and the Arduino endpoints.
/// Nothing is stored on disk. Restarting the app clears everything.
/// </summary>
public sealed class RobotStore(TimeProvider clock)
{
    public static readonly string[] Emotions =
        ["happy", "sad", "stressed", "angry", "tired", "bored"];

    private static readonly TimeSpan OfflineAfter = TimeSpan.FromSeconds(15);
    private static readonly TimeSpan CommandLifetime = TimeSpan.FromSeconds(60);

    private readonly object _gate = new();
    private RobotCommand? _last;
    private DateTimeOffset? _lastSeen;

    /// <summary>Raised after any change so the UI can refresh.</summary>
    public event Action? Changed;

    public RobotSnapshot GetSnapshot()
    {
        lock (_gate)
        {
            ExpireIfNeeded();
            return new RobotSnapshot(IsConnected(), _lastSeen, _last);
        }
    }

    /// <summary>Queues a command. The newest command replaces an older one that is still pending.</summary>
    public RobotCommand? Enqueue(string emotion)
    {
        var id = emotion.Trim().ToLowerInvariant();
        if (!Emotions.Contains(id)) return null;

        RobotCommand cmd;
        lock (_gate)
        {
            var now = clock.GetUtcNow();
            cmd = new RobotCommand(Guid.NewGuid(), id, now, now + CommandLifetime, CommandStatus.Pending);
            _last = cmd;
        }
        Changed?.Invoke();
        return cmd;
    }

    /// <summary>Called by the Arduino. Counts as a heartbeat. Returns null when nothing is waiting.</summary>
    public RobotCommand? NextForDevice()
    {
        RobotCommand? result;
        lock (_gate)
        {
            _lastSeen = clock.GetUtcNow();
            ExpireIfNeeded();
            result = _last is { Status: CommandStatus.Pending } ? _last : null;
        }
        Changed?.Invoke();
        return result;
    }

    /// <summary>Called by the Arduino to confirm a step. Returns false for an unknown command id.</summary>
    public bool Report(Guid id, CommandStatus status)
    {
        lock (_gate)
        {
            _lastSeen = clock.GetUtcNow();
            if (_last is null || _last.Id != id) return false;
            _last = _last with { Status = status };
        }
        Changed?.Invoke();
        return true;
    }

    private bool IsConnected() => _lastSeen is { } seen && clock.GetUtcNow() - seen < OfflineAfter;

    private void ExpireIfNeeded()
    {
        if (_last is { Status: CommandStatus.Pending } c && clock.GetUtcNow() > c.ExpiresAt)
            _last = c with { Status = CommandStatus.Expired };
    }
}
