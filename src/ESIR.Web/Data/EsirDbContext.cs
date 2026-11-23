using ESIR.Web.Data.Entities;
using Microsoft.EntityFrameworkCore;

namespace ESIR.Web.Data;

public class EsirDbContext(DbContextOptions<EsirDbContext> options) : DbContext(options)
{
	public DbSet<ESIRArduino> Robots => Set<ESIRArduino>();

	protected override void OnModelCreating(ModelBuilder modelBuilder)
	{
		modelBuilder.Entity<ESIRArduino>(robot =>
		{
			robot.HasKey(r => r.Id);
			robot.Property(r => r.Id).HasMaxLength(50);
			robot.Property(r => r.Name).HasMaxLength(100).IsRequired();
			robot.Property(r => r.KeyHash).HasMaxLength(64).IsRequired();
		});
	}
}