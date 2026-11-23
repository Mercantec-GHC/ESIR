using System.Security.Cryptography;
using System.Text;
using ESIR.Web.Services;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Filters;

namespace ESIR.Web.Api.Filters;

/// <summary>
/// Requires a valid X-Device-Key header matching the device identified by the {deviceId} route value.
/// Keys are read from the "Devices" configuration section.
/// </summary>
[AttributeUsage(AttributeTargets.Method | AttributeTargets.Class)]
public sealed class RequireDeviceKeyAttribute : Attribute, IActionFilter
{
	public const string KeyHeader = "X-Device-Key";

	public void OnActionExecuting(ActionExecutingContext context)
	{
		var http = context.HttpContext;
		var deviceId = http.Request.RouteValues["deviceId"] as string;

		var devices = http.RequestServices.GetRequiredService<IConfiguration>()
			.GetSection("Devices").Get<DeviceOptions[]>() ?? [];

		var device = devices.FirstOrDefault(d => d.Id == deviceId);
		if (device is null)
		{
			context.Result = new NotFoundResult();
			return;
		}

		var sentKey = http.Request.Headers[KeyHeader].ToString();
		if (string.IsNullOrEmpty(device.Key) || !KeysMatch(sentKey, device.Key))
		{
			context.Result = new UnauthorizedResult();
		}
	}

	public void OnActionExecuted(ActionExecutedContext context)
	{
	}

	private static bool KeysMatch(string sent, string expected) =>
		CryptographicOperations.FixedTimeEquals(Encoding.UTF8.GetBytes(sent), Encoding.UTF8.GetBytes(expected));
}
