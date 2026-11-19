using ESIR.Web.Services;
using Microsoft.AspNetCore.Components;

namespace ESIR.Web.Components.Pages;

public partial class MyEsir : IDisposable
{
    [Inject] private RobotStore Store { get; set; } = default!;

    private RobotSnapshot snapshot = new(false, null, null);
    private Timer? refresh;

    protected override void OnInitialized()
    {
        snapshot = Store.GetSnapshot();
        Store.Changed += OnStoreChanged;
        // Re-check every 5 s so the page shows "Offline" when the robot stops polling
        refresh = new Timer(_ => OnStoreChanged(), null, TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(5));
    }

    private void OnStoreChanged() => _ = InvokeAsync(() =>
    {
        snapshot = Store.GetSnapshot();
        StateHasChanged();
    });

    public void Dispose()
    {
        Store.Changed -= OnStoreChanged;
        refresh?.Dispose();
    }
}
