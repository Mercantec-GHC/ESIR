using Microsoft.AspNetCore.Components;

namespace ESIR.Web.Components.Shared;

public partial class RobotFace
{
    /// <summary>neutral, happy, excited, sad, calm, sleepy, tired, surprised, confused, angry, stressed or bored.</summary>
    [Parameter] public string Mood { get; set; } = "neutral";

    /// <summary>Size of the face as a CSS length, for example "16px".</summary>
    [Parameter] public string Size { get; set; } = "16px";
}