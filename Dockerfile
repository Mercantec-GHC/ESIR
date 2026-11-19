FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
WORKDIR /src
COPY ESIR.slnx ./
COPY src/ESIR.Web/ESIR.Web.csproj src/ESIR.Web/
RUN dotnet restore src/ESIR.Web/ESIR.Web.csproj
COPY src/ src/
RUN dotnet publish src/ESIR.Web/ESIR.Web.csproj -c Release -o /app --no-restore

FROM mcr.microsoft.com/dotnet/aspnet:10.0
WORKDIR /app
COPY --from=build /app .
ENV ASPNETCORE_URLS=http://+:8080
EXPOSE 8080
ENTRYPOINT ["dotnet", "ESIR.Web.dll"]


