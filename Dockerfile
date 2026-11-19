# Build stage
FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
WORKDIR /src
COPY src/ESIR.Web/ESIR.Web.csproj src/ESIR.Web/
RUN dotnet restore src/ESIR.Web/ESIR.Web.csproj
COPY src/ src/
RUN dotnet publish src/ESIR.Web/ESIR.Web.csproj -c Release -o /app --no-restore

# Run stage
FROM mcr.microsoft.com/dotnet/aspnet:10.0 AS runtime
WORKDIR /app
COPY --from=build /app .
# Run as the image's built-in non-root user
USER $APP_UID
ENV ASPNETCORE_HTTP_PORTS=8080
EXPOSE 8080
ENTRYPOINT ["dotnet", "ESIR.Web.dll"]
