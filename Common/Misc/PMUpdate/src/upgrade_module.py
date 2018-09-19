# coding: utf-8
import os
from shutil import copy2
from utility import convert_time_doy_to_string_format
import xml.etree.ElementTree
import datetime


def load_error_file_on_xml(errorfilename, pointingxmlfilename, receivername):
    """
    funzione per l'aggiornamento del file pointingxmlfilename passato con le informazioni dei parametri di puntamento presenti nel file errorfilename

    :param errorfilename: file con i paramenti di puntamento ottento tramite il programma pdplt
    :param pointingxmlfilename: file xml con i modelli di puntamento di nuraghe
    :param receivername:  nome del ricevitore per il quale si stanno aggiornando i dati di puntamento
    :return: tupla (x:int, description:string) con un codice di errore e la descrizione dell'errore quando x=0 nessun errore

    il file pointingxmlfilename viene salvato prima della sua modifica aggiungendo al nome del file una stringa con un timestamp

    """

    #verifica che i paramenti siano corretti
    # verifica che i file esistano
    try:
        if errorfilename:
            if os.path.isfile(errorfilename):
                pass
            else:
                return (1, "errorfilename is not a file")
        else:
            return (2, "errorfilename is not a valide file name")
    except:
        return (3, "errorfilename: generic error")

    try:
        if pointingxmlfilename:
            if os.path.isfile(pointingxmlfilename):
                pass
            else:
                return (4, "pointingxmlfilename is not a file")
        else:
            return (5, "pointingxmlfilename is not a valide file name")
    except:
        return (6, "pointingxmlfilename: generic error")

    #verifica che il file errorfilename abbia la stringa di inizio della sezione con i dati dei parametri
    #estrai i dati dei paramentri dal file

    try:
        errorfilename_rows = []
        start_section = False
        with open(errorfilename, 'rb') as f:
            for line in f:
                line_strip = line.strip()
                if start_section:
                    if line_strip == "*":
                        pass
                    else:
                        errorfilename_rows.append(line_strip.split())
                if line_strip == "$new_model":
                    start_section=True
            else:
                # No more lines to be read from file
                if not start_section:
                    return (7, "errorfilename: new model not found")
        if len(errorfilename_rows) < 8:
            return (7, "errorfilename: new model section too short")


        if len(errorfilename_rows[0]) <> 7 or  \
                len(errorfilename_rows[1]) <> 31 or \
                len(errorfilename_rows[2]) <> 5 or \
                len(errorfilename_rows[3]) <> 5 or \
                len(errorfilename_rows[4]) <> 5 or \
                len(errorfilename_rows[5]) <> 5 or \
                len(errorfilename_rows[6]) <> 5 or \
                len(errorfilename_rows[7]) <> 5:
            return (8, "errorfilename: not enough parameters in new model section")

        try:
            newmodel_date = convert_time_doy_to_string_format(errorfilename_rows[0])
        except RuntimeError, e:
            return (1001, "DOY convertion error: " + e.message)
        phi = errorfilename_rows[1][0]

        coefficientNum = errorfilename_rows[1][1:]

        coefficientVal = errorfilename_rows[2] + errorfilename_rows[3] + errorfilename_rows[4] + errorfilename_rows[5] + errorfilename_rows[6] + errorfilename_rows[7]

    except:
        return (9, "errorfilename: generic error on parameters extraction")

    #verifica che il file di pointingxmlfilename sia un file xml che siano presenti le sezioni dei ricevitori
    #salva l'elenco dei ricevitori presenti e salvali nella list receivers

    try:
        xml.etree.ElementTree.register_namespace('', 'urn:schemas-cosylab-com:PointingModel:1.0')
        xml.etree.ElementTree.register_namespace('baci', 'urn:schemas-cosylab-com:BACI:1.0')
        xml.etree.ElementTree.register_namespace('cdb', 'urn:schemas-cosylab-com:CDB:1.0')
        xml.etree.ElementTree.register_namespace('xsi', 'http://www.w3.org/2001/XMLSchema-instance')
        et = xml.etree.ElementTree.parse(pointingxmlfilename)

        e = et.getroot()
        receivers = []
        for child in e:
            receivers.append(child[0].text.strip())
        if len(receivers) == 0:
            return (10, "pointingxmlfilename: no receivers found")
    except:
        return (11, "pointingxmlfilename: xml parsing error")


    #verifica che receivername sia presente nella lista receivers
    receiver = receivername.strip()

    if not receiver in receivers:
        return (11, "receivername: recievername not present in pointingxmlfilename")

    #stampa in output i paramentri per il ricevitore che si vuole aggiornare

    print("New model date: {newmodel_date}".format(newmodel_date=newmodel_date))
    print("Phi: {phi}".format(phi=phi))
    print("coefficientNum: {coefficientNum}".format(coefficientNum=coefficientNum))
    print("coefficientVal: {coefficientVal}".format(coefficientVal=coefficientVal))

    #scrivi nel file pointingxmlfilename i nuovi paramentri estratti dal file errorfilename
    for child in e:
        if child[0].text.strip() == receiver:
            child[1].text = phi
            for i in range(2, len(coefficientVal)+2):
                child[i*2-2].text = coefficientNum[i-2]
                child[i*2-2+1].text = coefficientVal[i - 2]

    #salva il file pointingxmlfilename originale aggiungendo al nome del file una stringa con il time stamp

    nowstring = datetime.datetime.now().strftime("%Y%m%d%H%M%S")

    copy2(pointingxmlfilename, pointingxmlfilename + "_" + nowstring)
    print("Backup file: {backup_file}".format(backup_file=pointingxmlfilename + "_" + nowstring))

    #sovrascrive il vecchio file di pointing
    try:
        et.write(pointingxmlfilename,  encoding='ISO-8859-1', xml_declaration=True)
    except:
        return(14, "Error: Generic error overwriting xml pointing file")
    #nessun errore rilevato

    return (0, "")
