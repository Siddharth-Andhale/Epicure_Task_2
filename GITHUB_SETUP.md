# GitHub Upload Checklist & Instructions

## ✅ Files Ready for GitHub

### Root Level Files
- ✅ `.gitignore` - Excludes build artifacts, dependencies, IDE files
- ✅ `.gitattributes` - Ensures consistent line endings (LF) across platforms
- ✅ `README.md` - Comprehensive project overview and setup guide
- ✅ `PRD.md` - Detailed product requirements and specifications

### STM32 Project
```
Timer Delay Task/
├── Core/
│   ├── Inc/
│   │   ├── main.h ✅
│   │   ├── stm32f4xx_it.h ✅
│   │   └── stm32f4xx_hal_conf.h ✅
│   └── Src/
│       ├── main.c ✅ (Timer + UART ISR)
│       ├── stm32f4xx_it.c ✅ (Interrupt handlers)
│       └── system_stm32f4xx.c ✅
├── Drivers/ ✅
├── platformio.ini ✅
└── .project ✅
```

### ESP32 Project
```
ESP UART Reciever and MQTT Pub/
├── src/
│   └── main.cpp ✅ (157 lines, optimized)
├── include/
│   └── config.h ✅ (with TLS certificate)
├── lib/ ✅
├── platformio.ini ✅ (with dependencies)
└── .gitignore ✅
```

---

## 📋 Pre-Push Verification Checklist

- [ ] **Verify no secrets are exposed:**
  ```bash
  grep -r "password" . --include="*.h" --include="*.c" --include="*.cpp"
  grep -r "WIFI" . --include="*.h" --include="*.c" --include="*.cpp"
  ```
  Update any plaintext WiFi/MQTT credentials in `config.h` to placeholder values

- [ ] **Check file permissions:**
  ```bash
  git check-ignore -v *
  ```
  Should show build directories and dependencies being ignored

- [ ] **Verify .gitignore is working:**
  ```bash
  git status
  ```
  Should NOT show:
  - `.pio/` or `.platformio/` directories
  - `Debug/` or `Release/` directories
  - `*.o` or `*.a` files
  - `.vscode/` IDE files

- [ ] **Count files to be tracked:**
  ```bash
  git ls-files --others --exclude-standard
  ```
  Should only show source files, config, and documentation

---

## 🚀 GitHub Push Steps

### 1. Initialize Git Repository (First Time Only)
```bash
cd "Epicure Task 2"
git init
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

### 2. Add All Files
```bash
git add .
```

### 3. Create Initial Commit
```bash
git commit -m "Initial commit: STM32F407VET6 UART+LED Timer System with ESP32 MQTT Bridge"
```

### 4. Add Remote Repository (Replace with your repo)
```bash
git remote add origin https://github.com/YOUR_USERNAME/Epicure-Task-2.git
```

### 5. Push to GitHub
```bash
git branch -M main
git push -u origin main
```

---

## 📝 Commit Message Convention

Use clear, descriptive commit messages:

```
# Feature commits
git commit -m "feat: Add TLS/SSL MQTT encryption for secure cloud connection"
git commit -m "feat: Implement NTP time sync for accurate timestamps"

# Bug fixes
git commit -m "fix: Correct raw string literal syntax in config.h"
git commit -m "fix: Remove buffer overflow vulnerability in UART handler"

# Documentation
git commit -m "docs: Add comprehensive README and setup instructions"
git commit -m "docs: Update PRD with final timer calculations"

# Configuration
git commit -m "chore: Add .gitignore and .gitattributes for project"
```

---

## 📊 Repository Statistics

### Expected File Count
- Source files: ~15 files
- Configuration files: ~5 files
- Documentation: ~3 files
- Total tracked files: ~23 files

### Size Estimate
- STM32 project: ~2-3 MB (with drivers)
- ESP32 project: ~0.5 MB
- Documentation: ~0.1 MB
- **Total**: ~3 MB (reasonable for GitHub)

---

## 🔐 Security Checklist

Before pushing to GitHub:

- [ ] **Remove sensitive data:**
  - Replace actual WiFi SSID with placeholder
  - Replace WiFi password with placeholder
  - Replace MQTT username with placeholder
  - Replace MQTT password with placeholder
  - Keep only the certificate (it's public)

### Example config.h sanitization:
```cpp
// BEFORE (❌ DO NOT COMMIT)
#define WIFI_SSID "MERIDIAN 001_2.4G"
#define WIFI_PASSWORD "feb30mg@001"
#define MQTT_USER "Epicure_Task_Broker"
#define MQTT_PASS "Epicure#2025"

// AFTER (✅ COMMIT SAFELY)
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define MQTT_USER "Your_MQTT_Username"
#define MQTT_PASS "Your_MQTT_Password"
```

---

## 📖 Creating README Sections

The main `README.md` includes:
- ✅ Project overview
- ✅ Hardware setup diagram
- ✅ Configuration instructions
- ✅ Build & upload guide
- ✅ Verification steps
- ✅ Troubleshooting section
- ✅ Technical specifications
- ✅ Data flow diagram

---

## 🔄 Future Maintenance

### Adding New Features
```bash
git checkout -b feature/new-feature-name
# Make changes
git add .
git commit -m "feat: Description of new feature"
git push origin feature/new-feature-name
# Create Pull Request on GitHub
```

### Fixing Bugs
```bash
git checkout -b bugfix/issue-description
# Fix the bug
git add .
git commit -m "fix: Description of fix"
git push origin bugfix/issue-description
```

---

## 📚 .gitignore Coverage

The `.gitignore` file excludes:

| Category | Examples | Reason |
|----------|----------|--------|
| Build Output | `.pio/`, `Debug/`, `*.o` | Not source code |
| IDE Files | `.vscode/`, `.idea/` | User-specific |
| Dependencies | `node_modules/`, `.piolibdeps/` | Regenerate via package managers |
| Temporary | `*.bak`, `*.tmp`, `*.log` | Build artifacts |
| OS Files | `Thumbs.db`, `.DS_Store` | Platform-specific |
| Secrets | `credentials.h`, `.env` | Security |

---

## ✨ Final Checklist Before Push

- [ ] All source files are committed
- [ ] `.gitignore` is preventing build artifacts
- [ ] `.gitattributes` is set for line endings
- [ ] `README.md` is comprehensive and accurate
- [ ] `PRD.md` is complete
- [ ] No plaintext passwords in `config.h`
- [ ] No IDE settings exposed (except minimal ones)
- [ ] All documentation is up-to-date
- [ ] Code is properly commented
- [ ] No debug print statements left in production code
- [ ] File permissions are correct (755 for scripts, 644 for files)

---

## 🎉 Ready to Push!

```bash
git status                    # Verify only source files are shown
git log --oneline             # Verify commits
git push -u origin main       # Push to GitHub
```

---

**Last Updated**: November 26, 2025
**Status**: ✅ Ready for GitHub Upload
