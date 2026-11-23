using System.Security.Cryptography;
using System.Text;
using ESIR.Web.Services;

namespace ESIR.Web.Api;

/// <summary>Endpoints the Arduino calls. Every call must send the X-Device-Key header.</summary>
public static class DeviceEndpoints
{
    public const string KeyHeader = "X-Device-Key";

    public static IEndpointRouteBuilder MapDeviceApi(this IEndpointRouteBuilder app)
    {
        var device = app.MapGroup("/api/v1/devices/{deviceId}")
            .WithTags("Device (Arduino)")
            .AddEndpointFilter(RequireDeviceKey);

        // The robot asks for its next command. Also counts as a heartbeat.
        device.MapGet("/commands/next", (RobotStore store) =>
            store.NextForDevice() is { } cmd ? Results.Ok(cmd) : Results.NoContent());

        // The robot reports a step for a command: accepted, running, completed.
        device.MapPost("/status", (StatusReport report, RobotStore store) =>
            store.Report(report.CommandId, report.Status) ? Results.NoContent() : Results.NotFound());

        return app;
    }

    private static async ValueTask<object?> RequireDeviceKey(
        EndpointFilterInvocationContext context, EndpointFilterDelegate next)
    {
        var http = context.HttpContext;
        var deviceId = http.Request.RouteValues["deviceId"] as string;
        var devices = http.RequestServices.GetRequiredService<IConfiguration>()
            .GetSection("Devices").Get<DeviceOptions[]>() ?? [];

        var device = devices.FirstOrDefault(d => d.Id == deviceId);
        if (device is null) return Results.NotFound();

        var sentKey = http.Request.Headers[KeyHeader].ToString();
        if (string.IsNullOrEmpty(device.Key) || !KeysMatch(sentKey, device.Key))
            return Results.Unauthorized();

        return await next(context);
    }

    private static bool KeysMatch(string sent, string expected) =>
        CryptographicOperations.FixedTimeEquals(Encoding.UTF8.GetBytes(sent), Encoding.UTF8.GetBytes(expected));
}

public sealed record StatusReport(Guid CommandId, CommandStatus Status);
