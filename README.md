🤖 NutriBot

Projeto Brasileiro de Um Robô assistente de profissionais de nutrição.
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <title>Estúdio 3D - Robô Assistente (PNG Transparente)</title>
  <style>
    body {
      margin: 0;
      font-family: 'Segoe UI', sans-serif;
      /* Fundo xadrez apenas na visualização para mostrar que é transparente */
      background-color: #1e293b;
      background-image: 
        linear-gradient(45deg, #0f172a 25%, transparent 25%), 
        linear-gradient(-45deg, #0f172a 25%, transparent 25%), 
        linear-gradient(45deg, transparent 75%, #0f172a 75%), 
        linear-gradient(-45deg, transparent 75%, #0f172a 75%);
      background-size: 24px 24px;
      background-position: 0 0, 0 12px, 12px -12px, -12px 0;
      color: white;
      overflow: hidden;
    }
    #ui {
      position: fixed;
      top: 20px;
      left: 20px;
      background: rgba(15, 23, 42, 0.9);
      padding: 18px;
      border-radius: 16px;
      border: 1px solid rgba(0, 229, 255, 0.3);
      box-shadow: 0 10px 30px rgba(0,0,0,0.5);
      z-index: 10;
      width: 260px;
    }
    h3 { margin: 0 0 8px 0; color: #00e5ff; font-size: 16px; }
    p { margin: 0 0 14px 0; font-size: 12px; color: #94a3b8; }
    button {
      width: 100%;
      padding: 12px;
      margin-bottom: 8px;
      border: none;
      border-radius: 10px;
      font-weight: bold;
      cursor: pointer;
      font-size: 14px;
      transition: transform 0.1s;
    }
    button:active { transform: scale(0.97); }
    .btn-download {
      background: linear-gradient(135deg, #00e5ff, #0284c7);
      color: #05101a;
      font-size: 15px;
      box-shadow: 0 4px 15px rgba(0, 229, 255, 0.35);
    }
    .btn-angle {
      background: #334155;
      color: #f1f5f9;
    }
  </style>
</head>
<body>

  <div id="ui">
    <h3>📦 Estúdio 3D do Produto</h3>
    <p>Arraste com o mouse para girar o robô ou escolha um ângulo comercial:</p>
    <button class="btn-angle" onclick="setAngle(-0.45, 0.22)">💎 Ângulo Comercial (3/4)</button>
    <button class="btn-angle" onclick="setAngle(0, 0.08)">🎯 Visão Frontal</button>
    <button class="btn-angle" onclick="setAngle(0.5, -0.15)">🚀 Perspectiva Dinâmica</button>
    <button class="btn-download" onclick="downloadPNG()">📸 Baixar PNG Transparente</button>
  </div>

  <script src="https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js"></script>
  <script>
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(38, window.innerWidth / window.innerHeight, 0.1, 100);
    camera.position.set(0, 0, 11);

    // Renderizador com Canal Alpha (Fundo 100% Transparente)
    const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true, preserveDrawingBuffer: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
    renderer.setClearColor(0x000000, 0);
    document.body.appendChild(renderer.domElement);

    // Iluminação de Estúdio Comercial
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.75);
    scene.add(ambientLight);

    const keyLight = new THREE.DirectionalLight(0xffffff, 0.9);
    keyLight.position.set(6, 10, 8);
    scene.add(keyLight);

    const rimLight = new THREE.DirectionalLight(0x00e5ff, 0.4);
    rimLight.position.set(-8, -4, -5);
    scene.add(rimLight);

    const fillLight = new THREE.DirectionalLight(0xe2e8f0, 0.4);
    fillLight.position.set(-6, 2, 6);
    scene.add(fillLight);

    // Grupo do Produto 3D
    const robotGroup = new THREE.Group();
    scene.add(robotGroup);

    // Função para criar retângulo arredondado 3D
    function createRoundedRectShape(w, h, r) {
      const s = new THREE.Shape();
      s.moveTo(-w/2 + r, -h/2);
      s.lineTo(w/2 - r, -h/2);
      s.quadraticCurveTo(w/2, -h/2, w/2, -h/2 + r);
      s.lineTo(w/2, h/2 - r);
      s.quadraticCurveTo(w/2, h/2, w/2 - r, h/2);
      s.lineTo(-w/2 + r, h/2);
      s.quadraticCurveTo(-w/2, h/2, -w/2, h/2 - r);
      s.lineTo(-w/2, -h/2 + r);
      s.quadraticCurveTo(-w/2, -h/2, -w/2 + r, -h/2);
      return s;
    }

    // 1. Corpo Principal Branco
    const bodyShape = createRoundedRectShape(3.2, 5.4, 0.42);
    const bodyGeo = new THREE.ExtrudeGeometry(bodyShape, {
      depth: 1.4,
      bevelEnabled: true,
      bevelSegments: 8,
      steps: 1,
      bevelSize: 0.12,
      bevelThickness: 0.12
    });
    bodyGeo.center();
    const bodyMat = new THREE.MeshPhysicalMaterial({
      color: 0xf8fafc,
      roughness: 0.25,
      metalness: 0.02,
      clearcoat: 0.3,
      clearcoatRoughness: 0.2
    });
    const bodyMesh = new THREE.Mesh(bodyGeo, bodyMat);
    robotGroup.add(bodyMesh);

    // 2. Moldura Preta Texturizada Superior (Visor)
    const visorShape = createRoundedRectShape(2.85, 2.25, 0.28);
    const visorGeo = new THREE.ExtrudeGeometry(visorShape, {
      depth: 0.12,
      bevelEnabled: true,
      bevelSegments: 3,
      steps: 1,
      bevelSize: 0.03,
      bevelThickness: 0.03
    });
    visorGeo.center();
    const visorMat = new THREE.MeshStandardMaterial({
      color: 0x141417,
      roughness: 0.92,
      metalness: 0.05
    });
    const visorMesh = new THREE.Mesh(visorGeo, visorMat);
    visorMesh.position.set(0, 1.25, 0.80);
    robotGroup.add(visorMesh);

    // 3. Tela OLED Central com Olhos Felizes Brilhantes
    const oledCanvas = document.createElement('canvas');
    oledCanvas.width = 512;
    oledCanvas.height = 280;
    const ctx = oledCanvas.getContext('2d');

    // Fundo preto profundo do OLED
    ctx.fillStyle = '#030712';
    ctx.fillRect(0, 0, 512, 280);

    // Brilho e desenho dos olhos felizes (arcos estilo Cozmo)
    ctx.shadowColor = '#00e5ff';
    ctx.shadowBlur = 22;
    ctx.fillStyle = '#e0faff';

    function drawHappyEye(cx, cy) {
      ctx.beginPath();
      ctx.arc(cx, cy, 52, Math.PI, 0, false);
      ctx.quadraticCurveTo(cx + 52, cy + 12, cx + 30, cy + 8);
      ctx.quadraticCurveTo(cx, cy - 12, cx - 30, cy + 8);
      ctx.quadraticCurveTo(cx - 52, cy + 12, cx - 52, cy);
      ctx.closePath();
      ctx.fill();
    }
    drawHappyEye(195, 148);
    drawHappyEye(317, 148);

    const oledTex = new THREE.CanvasTexture(oledCanvas);
    const oledGeo = new THREE.PlaneGeometry(1.45, 0.85);
    const oledMat = new THREE.MeshBasicMaterial({ map: oledTex });
    const oledMesh = new THREE.Mesh(oledGeo, oledMat);
    oledMesh.position.set(0, 1.25, 0.895);
    robotGroup.add(oledMesh);

    // 4. Botão Oval Inferior com Ícone de Sino
    const btnCanvas = document.createElement('canvas');
    btnCanvas.width = 256;
    btnCanvas.height = 360;
    const bctx = btnCanvas.getContext('2d');
    bctx.fillStyle = '#f1f5f9';
    bctx.fillRect(0, 0, 256, 360);

    // Desenho do sino em baixo-relevo
    bctx.strokeStyle = '#cbd5e1';
    bctx.lineWidth = 8;
    bctx.beginPath();
    bctx.roundRect(16, 16, 224, 328, 110);
    bctx.stroke();

    // Ícone do sino invertido (como na foto)
    bctx.fillStyle = '#e2e8f0';
    bctx.strokeStyle = '#cbd5e1';
    bctx.lineWidth = 6;
    bctx.beginPath();
    bctx.moveTo(78, 135);
    bctx.lineTo(178, 135);
    bctx.lineTo(162, 205);
    bctx.quadraticCurveTo(128, 235, 94, 205);
    bctx.closePath();
    bctx.fill();
    bctx.stroke();

    bctx.beginPath();
    bctx.arc(128, 228, 12, 0, Math.PI * 2);
    bctx.fill();
    bctx.stroke();

    const btnTex = new THREE.CanvasTexture(btnCanvas);
    const btnShape = createRoundedRectShape(0.82, 1.25, 0.40);
    const btnGeo = new THREE.ExtrudeGeometry(btnShape, {
      depth: 0.04,
      bevelEnabled: true,
      bevelSegments: 4,
      steps: 1,
      bevelSize: 0.02,
      bevelThickness: 0.02
    });
    btnGeo.center();
    const btnMat = new THREE.MeshStandardMaterial({ map: btnTex, roughness: 0.35 });
    const btnMesh = new THREE.Mesh(btnGeo, btnMat);
    btnMesh.position.set(0, -1.25, 0.81);
    robotGroup.add(btnMesh);

    // Posição inicial em 3/4 Comercial
    let targetRotY = -0.45;
    let targetRotX = 0.22;
    robotGroup.rotation.y = targetRotY;
    robotGroup.rotation.x = targetRotX;

    // Interação de Mouse para girar
    let isDragging = false;
    let prevX = 0, prevY = 0;
    window.addEventListener('mousedown', e => {
      if (e.target.tagName === 'BUTTON') return;
      isDragging = true; prevX = e.clientX; prevY = e.clientY;
    });
    window.addEventListener('mouseup', () => isDragging = false);
    window.addEventListener('mousemove', e => {
      if (!isDragging) return;
      targetRotY += (e.clientX - prevX) * 0.01;
      targetRotX += (e.clientY - prevY) * 0.01;
      prevX = e.clientX; prevY = e.clientY;
    });

    function setAngle(y, x) {
      targetRotY = y;
      targetRotX = x;
    }

    function animate() {
      requestAnimationFrame(animate);
      robotGroup.rotation.y += (targetRotY - robotGroup.rotation.y) * 0.1;
      robotGroup.rotation.x += (targetRotX - robotGroup.rotation.x) * 0.1;
      renderer.render(scene, camera);
    }
    animate();

    function downloadPNG() {
      renderer.render(scene, camera);
      const link = document.createElement('a');
      link.download = 'robo-assistente-3d.png';
      link.href = renderer.domElement.toDataURL('image/png');
      link.click();
    }

    window.addEventListener('resize', () => {
      camera.aspect = window.innerWidth / window.innerHeight;
      camera.updateProjectionMatrix();
      renderer.setSize(window.innerWidth, window.innerHeight);
    });
  </script>
</body>
</html>

[index.html](https://github.com/user-attachments/files/33414815/index.html)



📖 Sobre o Projeto

Um robô que interage com a(o) nutricionista(o) e o(a) cliente, fazendo uma consulta dinâmica e que auxilia a(o) profissional na sua rotina de trabalho.

🛠️ Tecnologias Utilizadas

Linguagem C++ para Arduino (programado no Arduino IDE)
Feito em um ESP32
Display OLED I2C
...
...


🛠️ Software & APIs:

Arduino IDE e VS code

Libs:
ADAFRUIT GFX ( Para a tela OLED)
ESP32SERVO (Servo motores)
ESPASYNCSERVER ( Para gerar o captive portal de conexão )
ESP32 (Lib para funcionar o ESP32)
