namespace ESIR.Web.Components.Pages;

public partial class AboutUs
{
    private sealed record DayStep(string Title, string Text, string Mood);
    private sealed record Reaction(string Trigger, string What, string Sensor, string Mood);
    private sealed record FeelingRow(string Feeling, string Robot, string Website);

    private readonly DayStep[] _day =
    [
        new("Wake ESIR", "Touch it, pick it up, or pick a feeling on the website.", "sleepy"),
        new("Get a response", "ESIR answers with a face expression and LED light.", "happy"),
        new("Choose an activity", "For example a short breathing exercise or a cheering message.", "calm"),
        new("Pause or stop", "Pause whenever you like. Stop always wins.", "neutral"),
    ];

    private readonly Reaction[] _reactions =
    [
        new("Touch or hold", "Greets you, shows affection, or starts and pauses an activity.", "Sensor: the touch pads on the carrier", "happy"),
        new("Pick it up", "Surprised eyes, then an attentive look.", "Sensor: the accelerometer", "surprised"),
        new("Shake it gently", "A dizzy animation and an LED pattern.", "Sensor: the accelerometer, with a limit and a cooldown", "confused"),
        new("Tilt left or right", "The eyes look in the direction of the tilt.", "Sensors: the accelerometer and the gyroscope", "neutral"),
        new("Leave it alone", "Slow blinking, then sleepy eyes.", "Uses a timer for how long nothing happens", "sleepy"),
        new("Stop", "Cancels the activity and returns to quiet rest.", "A dedicated touch action, or the website", "calm"),
    ];

    private readonly FeelingRow[] _feelings =
    [
        new("Happy or excited", "Smiling eyes and a short celebration", "A cheering message and hearts"),
        new("Sad or lonely", "Gentle eyes, a heart and a steady light", "A reassuring message and hearts"),
        new("Stressed or worried", "A breathing cue with slowly pulsing lights", "A guided breathing animation"),
        new("Angry or frustrated", "A calm face and a slow light pattern", "Acknowledges the feeling and offers a pause"),
        new("Tired", "Sleepy eyes and a soft light", "Suggests a short break"),
        new("Bored", "Curious eyes and playful lights", "Suggests a simple activity"),
    ];

    private readonly string[] _states = ["Idle", "Responding", "Breathing", "Paused", "Sleeping"];
}