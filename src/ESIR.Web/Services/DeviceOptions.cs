namespace ESIR.Web.Services;

/// <summary>A robot that may call the device API. Bound from the "Devices" config section.</summary>
public sealed class DeviceOptions
{
    public string Id { get; set; } = "";

    /// <summary>Secret the robot sends in the X-Device-Key header. Set it with an environment variable, never in Git.</summary>
    public string Key { get; set; } = "";
}
