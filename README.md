## Sterowanie:

Strzał - Lewy przycisk myszy

Zmiana trybu ognia - V

## Założenia i ograniczenia

System został zaprojektowany jako minimalna, modularna architektura broni, skupiona na logice i jakości kodu, a nie na oprawie wizualnej.

Główne założenia:

- Rozdzielenie danych, logiki i wejścia gracza.
    
- System oparty na:
    
    - `WeaponDefinition` – dane statyczne broni.
        
    - `WeaponInstance` – logika runtime i stan broni.
        
    - `WeaponComponent` – punkt wejścia systemu.
        
- Komunikacja zdarzeniowa (delegaty) zamiast bezpośrednich zależności.
    
- System w pełni data-driven (tryby ognia, parametry broni w DataAsset).
    

Ograniczenia (świadome uproszczenia na potrzeby zadania):

- Brak animacji broni.
    
- Brak systemu przeładowania.

- Brak możliwości zmiany/usunięcia broni.

- Brak amunicji.
    
- Tylko jeden aktywny egzemplarz broni.
    
- Prosty system trafień oparty o line trace.
    

---

## Podział: C++ vs Blueprint

### Zrealizowane w C++

Cała logika gameplayowa:

- Architektura systemu broni:
    
    - `WeaponComponent`
        
    - `WeaponInstance`
        
    - `WeaponDefinition`
        
- Obsługa wejścia i przekazywanie komend do systemu broni.
    
- Logika strzelania:
    
    - cooldown
        
    - spread
        
    - line trace
        
    - aplikowanie obrażeń
        
- Obsługa trybów ognia (Semi / Auto – zależnie od definicji).
    
- System debugowego snapshotu stanu broni.
    
- Debug widget UMG sterowany z C++.
    
- Dummy z interfejsem przyjmowania obrażeń.
    

### Zrealizowane w Blueprint

Blueprinty użyte wyłącznie do:

- Konfiguracji postaci testowej oraz postaci celu.
    
- Definicji DataAssetów broni (`WeaponDefinition`).
    

Brak logiki gameplayowej w Blueprintach – wszystkie systemy broni są zaimplementowane w C++.
