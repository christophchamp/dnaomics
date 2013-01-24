#!/usr/bin/env bash
#== Test data with odd number of sample sizes==
pathway_id_list=P1,P2,P3,P4,P5
python go_pipe/go_pipe_notebook.py -c Control -s odds_ratio -t 1.0 -p $pathway_id_list example_data.tab example_metadata.tab example_pathway.tab stats.tab > log.txt
