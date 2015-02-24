# CrutemConvert
Converts Crutem station temperature data into various usable formats.

Current State:

Version: 0.4c

Input Format(s)
    Crutem 4 (pre 4.3.0) collated header/data text format


Output Format(s)
    CSV of unweighted global averages by year.

FEATURES:
    Global unweighted, mapped, averages from Crutem station data.
    Daily temperature interpolation on demand.
    Cross-platform (Haiku, Linux, Windows)

- - - - - - - -

Planned Support:

Input Format(s)
    Crutem 4 raw station files (incl 4.3.0)

Output Format(s)
    EarthModel StationList EMSL (EMSL) binary format.
    Direct console output.
    CSV of any selected station data.

Features:
    Coordinate cell-based average temperature weighting.
    Select station interrogation and analysis.
    C++11 Multi-threading.
    Station internal data infill.
    Station cross-calibration and correlation.
    
    Haiku Only:
        EarthModel shared memory area and IPC.
        Global map generation.
        Video file output.


NOTES:

This project will serve as the framework for evaluating other station
temperature datasets.

This source code does not currently have a license, however I expect to use
something akin to Creative Commons or MIT.  Please inquire for your project
needs.
