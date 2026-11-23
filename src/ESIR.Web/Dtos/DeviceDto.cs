namespace ESIR.Web.Dtos;

public class DeviceDto
{
	public Guid Id { get; set; }
	public string Name { get; set; } = string.Empty;
	public bool IsConnected { get; set; }
}
