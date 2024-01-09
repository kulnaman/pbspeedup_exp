import pandas as pd
import numpy as np
import argparse
def main():
    parser = argparse.ArgumentParser(description="cloverleaf experiments over power ranges and spatial for gpu and cpu")
    parser.add_argument('file',  type=str, help='input file path')
    parser.add_argument('-iter',type=int,help='iterations')
    args = parser.parse_args()
    files= args.file
    ite= args.iter

    df = pd.read_csv(files,sep='\t',index_col=False)
    conv_algo_column_time_name = 'FWD_FFT_TILING'
    conv_algo_column_workspace_name= f'{conv_algo_column_time_name} WORKSPACE'
    execution_time_row=df.loc[df[conv_algo_column_time_name]==df[conv_algo_column_time_name].max()]
    execution_time= execution_time_row[conv_algo_column_time_name].item()
    size = execution_time_row[conv_algo_column_workspace_name].item()
    print(execution_time_row)
    print(size)
    print(execution_time_row.to_json())
    print(f"{execution_time} time")
    print(f"Execution Time(s) one iter(s): {execution_time*1e-6}\n Total Execution time(s):{execution_time*1e-6*ite}\n Problem Size(MB): {size/1048576.0}")
if __name__ == "__main__":
    main()
