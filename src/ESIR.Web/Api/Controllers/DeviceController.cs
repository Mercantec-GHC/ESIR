using ESIR.Web.Api.Filters;
using ESIR.Web.Dtos;
using ESIR.Web.Models;
using ESIR.Web.Services;
using Microsoft.AspNetCore.Mvc;

namespace ESIR.Web.Api.Controllers;

[Route("api/[controller]")]
[ApiController]
public class DeviceController(RobotStore store) : ControllerBase
{
	// Temporary in-memory data. Replace with a database/service lookup later.
	private static readonly DeviceViewModel[] Devices =
	[
		new() { Id = Guid.NewGuid(), Name = "Device 1", IsConnected = true, LastSeen = DateTimeOffset.UtcNow },
		new() { Id = Guid.NewGuid(), Name = "Device 2", IsConnected = false, LastSeen = null },
	];

	[HttpGet]
	public ActionResult<IEnumerable<DeviceDto>> GetAllDevices()
	{
		var devices = Devices.Select(d => new DeviceDto
		{
			Id = d.Id,
			Name = d.Name,
			IsConnected = d.IsConnected,
		});

		return Ok(devices);
	}

	[HttpGet("{id}")]
	public ActionResult<DeviceDto> GetDeviceById(Guid id)
	{
		var device = Devices.FirstOrDefault(d => d.Id == id);
		if (device is null)
		{
			return NotFound();
		}
		var deviceDto = new DeviceDto
		{
			Id = device.Id,
			Name = device.Name,
			IsConnected = device.IsConnected,
		};
		return Ok(deviceDto);
	}

	// The robot asks for its next command. Also counts as a heartbeat.
	// GET api/v1/devices/{deviceId}/commands/next
	[HttpGet("/api/v1/devices/{deviceId}/commands/next")]
	[RequireDeviceKey]
	public IActionResult GetNextCommand(string deviceId)
	{
		return store.NextForDevice() is { } cmd ? Ok(cmd) : NoContent();
	}

	// The robot reports a step for a command: accepted, running, completed.
	// POST api/v1/devices/{deviceId}/status
	[HttpPost("/api/v1/devices/{deviceId}/status")]
	[RequireDeviceKey]
	public IActionResult ReportStatus(string deviceId, [FromBody] StatusReport report)
	{
		return store.Report(report.CommandId, report.Status) ? NoContent() : NotFound();
	}
}

public sealed record StatusReport(Guid CommandId, CommandStatus Status);

