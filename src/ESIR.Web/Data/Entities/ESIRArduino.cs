namespace ESIR.Web.Data.Entities;

public class ESIRArduino
{
	public string Id { get; set; } = string.Empty;
	public string Name { get; set; } = string.Empty;
	public string KeyHash { get; set; } = string.Empty;
	public DateTimeOffset CreatedAt { get; set; }
}
