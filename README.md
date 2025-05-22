<h1>Time &amp; Temp ESP32 Device</h1>
<p>An ESP32-based <strong>Time &amp; Temp</strong> display.<br>
Tap to switch between clock and weather.</p>

<h2>📋 Prerequisites &amp; Overview</h2>
<h3>Hardware:</h3>
<ul>
  <li>ESP32 Dev Board (e.g. WROOM32)</li>
  <li>2.8″ ILI9341 SPI TFT Display</li>
  <li>Female-to-female jumper wires</li>
  <li>Breadboard (required)</li>
  <li>Micro-USB cable (to connect ESP32 to your computer)</li>
</ul>
<h3>Software:</h3>
<ul>
  <li>Arduino IDE (1.8.x or 2.0+)</li>
  <li>Libraries via <em>Sketch → Include Library → Manage Libraries</em>:
    <ul>
      <li><strong>TFT_eSPI</strong> by Bodmer</li>
      <li><strong>ArduinoJson</strong> by Benoît Blanchon</li>
    </ul>
  </li>
</ul>

<h2>🔌 Wiring</h2>
<table>
  <thead>
    <tr><th>TFT Pin</th><th>ESP32 Pin</th><th>Notes</th></tr>
  </thead>
  <tbody>
    <tr><td>VCC</td><td>3.3 V</td><td><strong>Do not</strong> use 5 V</td></tr>
    <tr><td>GND</td><td>GND</td><td>—</td></tr>
    <tr><td>CS</td><td>GPIO 5</td><td>Chip Select</td></tr>
    <tr><td>DC</td><td>GPIO 16</td><td>Data/Command</td></tr>
    <tr><td>RST</td><td>GPIO 17</td><td>Reset</td></tr>
    <tr><td>MOSI</td><td>GPIO 23</td><td>SPI MOSI</td></tr>
    <tr><td>SCLK</td><td>GPIO 18</td><td>SPI Clock</td></tr>
    <tr><td>MISO</td><td>GPIO 19</td><td>SPI MISO (unused)</td></tr>
    <tr><td>LED</td><td>3.3 V</td><td>Backlight (tied HIGH)</td></tr>
    <tr><td>T_CS</td><td>GPIO 21</td><td>Touch CS (optional)</td></tr>
    <tr><td>T_IRQ</td><td>GPIO 4</td><td>Touch IRQ (optional)</td></tr>
  </tbody>
</table>

<h2>💾 Installation</h2>
<ol>
  <li>Clone or download this repo.</li>
  <li>Open <code>TimeWeather/TimeWeather.ino</code> in Arduino IDE.</li>
  <li>Edit your <strong>Wi-Fi credentials</strong> at the top of the sketch.</li>
  <li>Confirm timezone is set to your area or the area you prefer.</li>
  <li>Verify wiring matches the table above unless you have your own setup.</li>
  <li>Use the Micro-USB data cable to connect your ESP32 to your computer.</li>
  <li>Click <strong>Upload</strong> in the IDE.</li>
</ol>

<h2>🚀 Usage</h2>
<ul>
  <li><strong>Tap</strong> screen → toggle Clock ⇄ Weather.</li>
  <li><strong>Long-press</strong> (3 s) → Easter Egg.</li>
  <li>Weather auto-refreshes every 10 minutes.</li>
</ul>
