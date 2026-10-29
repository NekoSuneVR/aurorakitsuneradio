# AuroraKitsuneRadio browser companion player

A lightweight standalone web page for listening to the same radio catalogue while playing games that do not yet have a native addon.

## Run locally (no compilation)

Option A: open `index.html` in a desktop browser. Some browsers restrict scripts or external API calls from `file://` pages.

Option B (recommended): from this folder run a local static server:

```sh
python3 -m http.server 8080
```

Open `http://127.0.0.1:8080` in your browser. On Windows, use `py -m http.server 8080` if Python is installed under the `py` launcher.

## Use

1. Type a station name, country or genre to filter the API catalogue.
2. Browse results using **Previous** and **Next**.
3. Click **Listen** next to a station; use the browser's audio bar for volume/pause/stop.

The page fetches `https://aurora-kitsune.fumikoecho.ca/api/stations`, plays streams using `<audio>`, and does not need Node.js, npm or a build command.

## Deployment and caveats

You can host this directory as static files behind Nginx or another HTTPS web server. Ensure that the API allows the browser's origin with appropriate **CORS** headers; clients cannot fix server-side CORS restrictions themselves. Some live streams may be unsupported by the browser's codec engine or subject to geographic restrictions.

Do not publish the static page under an unrelated origin and assume cross-origin playback will work without testing. Now-playing metadata is not implemented. This is source-only and has not been browser-tested in this PR.
