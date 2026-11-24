using ESIR.Web.Components.Shared;

namespace ESIR.Web.Components.Pages;

public partial class Relax
{
    private readonly ExerciseStep[] _grounding =
    [
        new("See 5 things", "Look around and name five things you can see."),
        new("Feel 4 things", "Notice four things you can feel: your feet on the floor, your clothes, the chair, the air on your skin."),
        new("Hear 3 things", "Listen for three sounds, near or far."),
        new("Smell 2 things", "Find two things you can smell. If you can't smell anything, think of two smells you like."),
        new("Taste 1 thing", "Notice one taste in your mouth, or take one slow sip of water."),
    ];

    private readonly ExerciseStep[] _release =
    [
        new("Shoulders", "Lift your shoulders up to your ears, hold for a moment, then let them drop."),
        new("Jaw", "Let your teeth part a little and let your tongue rest."),
        new("Hands", "Make two fists, squeeze for three seconds, then spread your fingers wide."),
        new("Belly", "Let your belly go soft as you breathe out."),
        new("Face", "Soften your forehead, your eyes and your cheeks."),
    ];
}