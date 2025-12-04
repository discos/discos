#!/usr/bin/env python

import re
import sys
import numpy as np
import os

# --- COSTANTI DI CONVERSIONE ---
K_Y = 0.990268
K_DEN_RY = 2068.0
K_DEN_RX1 = 1791.0
K_DEN_RX23 = 3582.0


def estrai_coefficienti_debug(polinomio_stringa):

    """
    Estrae i coefficienti c0, c1, c2, c3, c4 da una stringa polinomiale.
    Gestisce caratteri sconosciuti, virgole e il termine noto (c0).
    """
    # Debug print:
    #print(f"polinomio_stringa: {polinomio_stringa}")

    # 1. Pulizia e Preparazione
    pulito = polinomio_stringa.split('=', 1)[-1].strip()
    pulito = pulito.replace(' ', '').replace(',', '.')
    
    # Risoluzione del carattere nascosto ''
    pulito = re.sub(r'(El)[^\^\+\-]', r'\1', pulito)
    
    # Debug print:
    #print(f"pulito: {pulito}")

    # 2. Inizializzazione e Pattern
    coeffs = {0: 0.0, 1: 0.0, 2: 0.0, 3: 0.0, 4: 0.0}
    pattern = re.compile(r'([+-]?)(\d*\.?\d*)\*?El\^?(\d)?', re.IGNORECASE)

    # 3. Estrazione dei Termini (c1 a c4)
    ultimaposizione_El = -1
    for match in pattern.finditer(pulito):
        segno, numero_str, potenza_str = match.groups()
        
        # Debug print:
        #print(f"segno, numero_str, potenza_str: {segno},{numero_str},{potenza_str}")
        
        ultimaposizione_El = match.end() # Aggiorna l'ultima posizione di match

        # Logica di conversione (invariata)
        if numero_str == '':
            numero = 1.0 if segno != '-' else -1.0
        else:
            numero = float(numero_str) * (-1.0 if segno == '-' else 1.0)
        
        potenza = 1 if potenza_str is None else int(potenza_str)

        if potenza in coeffs:
            coeffs[potenza] = numero

    # 4. Estrazione del Termine Noto (c0)
    
    # Cerca l'ultimo segno di addizione/sottrazione (che separa El dal c0)
    # Nota: Stiamo assumendo che c0 sia l'ultimo elemento della stringa.
    if ultimaposizione_El != -1:
        # Cerca l'ultimo segno + o - dopo l'ultimo termine 'El'
        
        # Trova la parte della stringa che segue l'ultimo match (potrebbe includere il segno)
        resto_stringa = pulito[ultimaposizione_El:]
        
        # Cerca il pattern del numero/segno all'inizio di questa sottostringa
        c0_pattern = re.compile(r'([+-]?\d+\.?\d*)$') # Cerca numero con segno alla FINE della stringa
        c0_match = c0_pattern.search(resto_stringa)

        if c0_match:
            coeffs[0] = float(c0_match.group(1))

    # 5. Restituzione
    return [coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4]]

def convert_polinomials(input_file, output_file='OutputFile.txt'):
    """
    Legge i polinomi dal file di input, applica le trasformazioni e scrive l'output.
    Versione ottimizzata per la gestione dei blocchi.
    """
    try:
        with open(input_file, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"ERRORE: File di input non trovato: {input_file}")
        sys.exit(1)

    # 1. Suddivisione dei blocchi basata sui marcatori Px (più robusta)
    # L'espressione cerca un marcatore di blocco (P seguito da numeri) e divide il contenuto.
    # Usiamo re.split per catturare il marcatore stesso.
    blocks = re.split(r'(P\d+)', content.strip())
    
    # Il primo elemento (indice 0) è il testo prima di P0, che può essere vuoto.
    # I nomi dei blocchi sono a indice dispari (1, 3, 5, ...), i contenuti a indice pari (2, 4, 6, ...)
    block_names = blocks[1::2]
    block_contents = blocks[2::2]

    output_lines = []
    processed_blocks = 0 

    for name, content in zip(block_names, block_contents):
        
        polys = {}
        
        # 2. Processa ciascuna riga nel blocco
        # Utilizziamo un pattern più semplice per identificare una riga di polinomio
        for line in content.split('\n'):
            line = line.strip()
            
            # Pattern che cerca una delle variabili seguita da '='
            match = re.match(r'(Z[123]|X|Y)\s*=\s*.*', line)
            
            if match:
                var = match.group(1)
                #coeffs = parse_polynomial_string(line)
                coeffs = estrai_coefficienti_debug(line)
                #print(f"var,coeff: {var}, {coeffs}")
                polys[var] = coeffs
        
        # <<< INIZIO STAMPA DI DEBUG >>>
        print(f"\n--- Blocco {name} ---")
        if len(polys) == 5:
            print("Status: ? 5 Polinomi Trovati. Coefficienti estratti (c0, c1, c2, c3, c4):")
            for var, coeffs in polys.items():
                coeffs_formatted = ', '.join([f"{c:.6e}" for c in coeffs])
                print(f"  {var} = [{coeffs_formatted}]")
        else:
            print(f"Status: ? Errore di Parsing. Trovati solo {len(polys)}/5 polinomi. BLOCCO SALTATO.")
            if len(polys) > 0:
                print("   Polinomi parziali trovati:", list(polys.keys()))
        # <<< FINE STAMPA DI DEBUG >>>
        
        
        if len(polys) < 5:
            continue

        processed_blocks += 1
        
        # --- APPLICAZIONE DELLE RELAZIONI ---
        # (Applico l'approssimazione  che arctg(a)=a, se a è abbastanza piccolo)
        
        coeffs_Ry = (np.array(polys['Z2']) - np.array(polys['Z3'])) / K_DEN_RY
        coeffs_Rx = (-np.array(polys['Z1']) / K_DEN_RX1 -np.array(polys['Z2']) / K_DEN_RX23 -np.array(polys['Z3']) / K_DEN_RX23)
        coeffs_x = np.array(polys['X'])
        coeffs_y = np.array(polys['Y']) * K_Y
        coeffs_z = (np.array(polys['Z3'])/3) + (np.array(polys['Z2'])/3) + (np.array(polys['Z1'])/3) - 0.139173101 

        output_polys = { 'Ry': coeffs_Ry, 'Rx': coeffs_Rx, 'x': coeffs_x, 'y': coeffs_y, 'z': coeffs_z }
        
        # --- SCRITTURA DELLE LINEE DI OUTPUT ---
        output_lines.append(name)
        for var, coeffs in output_polys.items():
            # Modifica qui: rimosso .replace('.', ',') per mantenere il punto decimale
            coeffs_formatted = ' '.join([f"{c:.15e}" for c in coeffs]) 
            output_lines.append(f"{var} = {coeffs_formatted}")
        output_lines.append('')

    # Scrittura finale del file di output
    if processed_blocks > 0:
        try:
            with open(output_file, 'w') as f:
                f.write('\n'.join(output_lines).strip())
            print(f"\n? Conversione completata. I risultati di {processed_blocks} blocco/i sono stati salvati in {output_file}")
        except IOError:
            print(f"\nERRORE: Impossibile scrivere il file di output: {output_file}")
            sys.exit(1)
    else:
        print("\n? NESSUN BLOCCO VALIDO PROCESSATO. Il file di output non è stato creato/aggiornato.")



# --- FUNZIONE PRINCIPALE PER L'ESECUZIONE DA LINEA DI COMANDO ---

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python poly_converter.py <nome_file_input.txt>")
        sys.exit(1)
    
    input_file_name = sys.argv[1]
    convert_polinomials(input_file_name)
