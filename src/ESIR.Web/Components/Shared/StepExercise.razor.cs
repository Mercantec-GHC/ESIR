using Microsoft.AspNetCore.Components;

namespace ESIR.Web.Components.Shared;

/// <summary>One step of a guided exercise.</summary>
public sealed record ExerciseStep(string Title, string Text);

public partial class StepExercise
{
    [Parameter, EditorRequired] public string Title { get; set; } = "";
    [Parameter] public string? Intro { get; set; }
    [Parameter, EditorRequired] public IReadOnlyList<ExerciseStep> Steps { get; set; } = [];

    // -1 = not started, Steps.Count = finished
    private int index = -1;

    private void Begin() => index = 0;
    private void Next() => index++;
    private void Back() { if (index > 0) index--; }
    private void Restart() => index = -1;
}