using System.Text.Json;
using System.Text.Json.Serialization;
using ESIR.Web.Components;
using ESIR.Web.Services;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddRazorComponents().AddInteractiveServerComponents();
builder.Services.AddSingleton(TimeProvider.System);
builder.Services.AddSingleton<RobotStore>();
builder.Services.ConfigureHttpJsonOptions(o =>
    o.SerializerOptions.Converters.Add(new JsonStringEnumConverter(JsonNamingPolicy.CamelCase)));

var app = builder.Build();

app.UseAntiforgery();

// ---- API: website side ----
var robot = app.MapGroup("/api/robot");
robot.MapGet("/state", (RobotStore store) => store.GetSnapshot());
robot.MapPost("/emotion", (EmotionRequest req, RobotStore store) =>
    store.Enqueue(req.Emotion ?? "") is { } cmd
        ? Results.Ok(cmd)
        : Results.ValidationProblem(new Dictionary<string, string[]>
          { ["emotion"] = [$"Unknown emotion. Use one of: {string.Join(", ", RobotStore.Emotions)}"] }));

// ---- API: Arduino side (polling) ----
var device = app.MapGroup("/api/device");
device.MapGet("/commands/next", (RobotStore store) =>
    store.NextForDevice() is { } cmd ? Results.Ok(cmd) : Results.NoContent());
device.MapPost("/status", (StatusReport report, RobotStore store) =>
    store.Report(report.CommandId, report.Status) ? Results.NoContent() : Results.NotFound());

app.MapRazorComponents<App>().AddInteractiveServerRenderMode();

app.Run();

record EmotionRequest(string? Emotion);
record StatusReport(Guid CommandId, CommandStatus Status);

