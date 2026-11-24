using System.Text.Json;
using System.Text.Json.Serialization;
using ESIR.Web.Components;
using ESIR.Web.Data;
using ESIR.Web.Services;
using Microsoft.EntityFrameworkCore;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddRazorComponents().AddInteractiveServerComponents();
builder.Services.AddControllers();
builder.Services.AddOpenApi();
builder.Services.AddSingleton(TimeProvider.System);
builder.Services.AddSingleton<RobotStore>();
var connectionString = builder.Configuration.GetConnectionString("Default");
if (string.IsNullOrWhiteSpace(connectionString))
	throw new InvalidOperationException(
		"ConnectionStrings:Default is not set. Locally: set it with dotnet user-secrets (see README). In Docker: set POSTGRES_PASSWORD in .env.");
builder.Services.AddDbContext<EsirDbContext>(o =>
	o.UseNpgsql(builder.Configuration.GetConnectionString("Default"),
		npgsql => npgsql.EnableRetryOnFailure()));
builder.Services.ConfigureHttpJsonOptions(o =>
    o.SerializerOptions.Converters.Add(new JsonStringEnumConverter(JsonNamingPolicy.CamelCase)));

var app = builder.Build();

using (var scope = app.Services.CreateScope())
{
	scope.ServiceProvider.GetRequiredService<EsirDbContext>().Database.Migrate();
}

app.UseAntiforgery();

// Swagger: OpenAPI document at /openapi/v1.json, UI at /swagger (all environments)
app.MapOpenApi();
app.UseSwaggerUI(o => o.SwaggerEndpoint("/openapi/v1.json", "ESIR API v1"));

app.MapStaticAssets();

// Health check for Docker and Dokploy
app.MapGet("/healthz", () => Results.Text("ok"));

// MVC controllers (e.g. Api/Controllers/*Controller.cs)
app.MapControllers();

app.MapRazorComponents<App>().AddInteractiveServerRenderMode();

app.Run();
