using Microsoft.AspNetCore.Components;

namespace ESIR.Web.Components.Shared;

public partial class PageHeader
{
    [Parameter, EditorRequired] public string Title { get; set; } = "";

    /// <summary>The mood of the small ESIR next to the title.</summary>
    [Parameter] public string Mood { get; set; } = "happy";

    /// <summary>Optional intro text under the title.</summary>
    [Parameter] public RenderFragment? ChildContent { get; set; }
}