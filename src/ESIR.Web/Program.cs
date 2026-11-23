using System.Text.Json;
using System.Text.Json.Serialization;
using ESIR.Web.Api;
using ESIR.Web.Components;
using ESIR.Web.Services;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddRazorComponents().AddInteractiveServerComponents();
builder.Services.AddOpenApi();
builder.Services.AddSingleton(TimeProvider.System);
builder.Services.AddSingleton<RobotStore>();
builder.Services.ConfigureHttpJsonOptions(o =>
    o.SerializerOptions.Converters.Add(new JsonStringEnumConverter(JsonNamingPolicy.CamelCase)));

var app = builder.Build();

app.UseAntiforgery();

// Swagger: OpenAPI document at /openapi/v1.json, UI at /swagger (Development only)
if (app.Environment.IsDevelopment())
{
    app.MapOpenApi();
    app.UseSwaggerUI(o => o.SwaggerEndpoint("/openapi/v1.json", "ESIR API v1"));
}
app.MapStaticAssets();

// Health check for Docker and Dokploy
app.MapGet("/healthz", () => Results.Text("ok"));

// Endpoints the Arduino calls (the website pages use RobotStore directly)
app.MapDeviceApi();

app.MapRazorComponents<App>().AddInteractiveServerRenderMode();

app.Run();
