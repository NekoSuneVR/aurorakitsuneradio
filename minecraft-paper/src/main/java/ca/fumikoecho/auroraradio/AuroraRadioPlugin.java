package ca.fumikoecho.auroraradio;

import com.google.gson.*;

import net.kyori.adventure.text.Component;
import net.kyori.adventure.text.event.ClickEvent;
import org.bukkit.plugin.java.JavaPlugin;
import com.craftmend.openaudiomc.api.interfaces.ClientApi;
import com.craftmend.openaudiomc.api.interfaces.MediaApi;
import com.craftmend.openaudiomc.api.clients.Client;
import com.craftmend.openaudiomc.api.media.Media;
import org.bukkit.command.*;
import java.net.URI;
import java.net.http.*;
import java.time.Duration;
import java.util.*;
import java.util.concurrent.CompletableFuture;

public final class AuroraRadioPlugin extends JavaPlugin implements CommandExecutor {
    private static final String MEDIA_ID = "aurora-radio";
    private static final String BASE = "https://aurora-kitsune.fumikoecho.ca";
    private final HttpClient client = HttpClient.newBuilder().connectTimeout(Duration.ofSeconds(10)).build();
    private volatile List<Station> stations = List.of();
    private final Map<UUID, List<Station>> lastResults = new HashMap<>();
    private record Station(String name, String country, String genre, String path) {}

    @Override public void onEnable() {
        Objects.requireNonNull(getCommand("radio")).setExecutor(this);
        updateStations();
    }

    private void updateStations() {
        HttpRequest request = HttpRequest.newBuilder(URI.create(BASE + "/api/stations"))
            .timeout(Duration.ofSeconds(20)).GET().build();
        client.sendAsync(request, HttpResponse.BodyHandlers.ofString()).thenAccept(response -> {
            if (response.statusCode() != 200) throw new IllegalStateException("HTTP " + response.statusCode());
            JsonArray array = JsonParser.parseString(response.body()).getAsJsonObject().getAsJsonArray("stations");
            if (array == null) throw new IllegalStateException("Missing stations array");
            List<Station> found = new ArrayList<>();
            for (JsonElement value : array) {
                try {
                    JsonObject s = value.getAsJsonObject();
                    if (!s.has("enabled") || !s.get("enabled").getAsBoolean()) continue;
                    String path = s.get("stream_path").getAsString();
                    String name = s.get("name").getAsString();
                    if (!path.startsWith("/stream/") || path.contains("..") || path.contains("?") || path.contains("#") || path.contains("\\") || name.isBlank()) continue;
                    found.add(new Station(name, text(s,"country"), text(s,"genre"),path));
                } catch (RuntimeException ignored) { /* Skip malformed entries */ }
            }
            stations = List.copyOf(found);
            getLogger().info("Loaded " + stations.size() + " Aurora radio stations");
        }).exceptionally(error -> { getLogger().warning("Radio API refresh failed: " + error.getMessage()); return null; });
    }
    private static String text(JsonObject o, String field) { return o.has(field) && !o.get(field).isJsonNull() ? o.get(field).getAsString() : ""; }
    @Override public boolean onCommand(CommandSender sender, Command command, String label, String[] args) {
        if (args.length > 0 && args[0].equalsIgnoreCase("refresh")) {
            if (!sender.hasPermission("auroraradio.refresh")) { sender.sendMessage("No permission."); return true; }
            updateStations(); sender.sendMessage("Fetching Aurora radio stations asynchronously."); return true;
        }
        if (args.length > 0 && args[0].equalsIgnoreCase("stop")) {
            if (!(sender instanceof org.bukkit.entity.Player p)) return true;
            if (getServer().getPluginManager().isPluginEnabled("OpenAudioMc")) {
                Client target = ClientApi.getInstance().getClient(p.getUniqueId());
                if (target != null) MediaApi.getInstance().stopFor(MEDIA_ID, target);
            }
            sender.sendMessage("Aurora radio stopped.");
            return true;
        }
        if (args.length > 0 && args[0].equalsIgnoreCase("play")) {
            if (!(sender instanceof org.bukkit.entity.Player p)) { sender.sendMessage("Use in game."); return true; }
            List<Station> results = lastResults.get(p.getUniqueId());
            int index;
            try { index = Integer.parseInt(args.length > 1 ? args[1] : "0") - 1; }
            catch (NumberFormatException e) { sender.sendMessage("Usage: /radio play <number>"); return true; }
            if (results == null || index < 0 || index >= results.size()) { sender.sendMessage("Search first with /radio <name>, then /radio play <number>."); return true; }
            Station station = results.get(index);
            // Minecraft cannot play external HTTP audio via a server-only plugin.
            String url = BASE + station.path();
            if (!getServer().getPluginManager().isPluginEnabled("OpenAudioMc")) {
                sender.sendMessage("OpenAudioMc must be installed and configured to listen in-game.");
                return true;
            }
            Client target = ClientApi.getInstance().getClient(p.getUniqueId());
            if (target == null || !target.isConnected()) { sender.sendMessage("Connect to OpenAudioMc audio session first using /audio."); return true; }
            MediaApi.getInstance().stopFor(MEDIA_ID, target);
            Media media = new Media(url);
            media.setMediaId(MEDIA_ID);
            media.setVolume(50);
            MediaApi.getInstance().playFor(media, target);
            sender.sendMessage("Playing " + station.name() + " via OpenAudioMc.");
            return true;
        }
        String query = args.length == 0 || args[0].equalsIgnoreCase("list") ? "" : String.join(" ",args).toLowerCase(Locale.ROOT);
        List<Station> matches = stations.stream().filter(s -> (s.name() + " " + s.country() + " " + s.genre()).toLowerCase(Locale.ROOT).contains(query)).limit(20).toList();
        if (sender instanceof org.bukkit.entity.Player p) lastResults.put(p.getUniqueId(), matches);
        sender.sendMessage("Aurora Radio: " + stations.size() + " stations. Showing " + matches.size() + " results (max 20). Web player: " + BASE);
        for (int i=0; i<matches.size(); i++) sender.sendMessage((i+1) + ". " + matches.get(i).name() + " (" + matches.get(i).country() + ") — /radio play " + (i+1));
        return true;
    }
}
