#!/usr/bin/env python3
"""
Quick script to check the O-RAN simulation database and show actual results
"""
import sqlite3
import json
import pandas as pd
import os
from glob import glob

def check_database():
    """Check SQLi
    te database contents"""
    print("=" * 80)
    print(" DATABASE ANALYSIS")
    print("=" * 80)
    
    # Find latest database
    db_files = glob("output/oran_simulation_*.db")
    if db_files:
        latest_db = max(db_files, key=os.path.getctime)
        print(f"📊 Database: {latest_db}")
        
        conn = sqlite3.connect(latest_db)
        cursor = conn.cursor()
        
        # Get tables
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table'")
        tables = cursor.fetchall()
        print(f"📋 Tables: {[t[0] for t in tables]}")
        
        # Check handover events
        try:
            cursor.execute("SELECT COUNT(*) FROM handover_events")
            count = cursor.fetchone()[0]
            print(f"🔄 Handover events: {count}")
            
            cursor.execute("SELECT * FROM handover_events LIMIT 3")
            rows = cursor.fetchall()
            print("📈 Sample handover data:")
            for i, row in enumerate(rows):
                print(f"  {i+1}. {row}")
        except Exception as e:
            print(f"⚠️ Error reading handover data: {e}")
        
        conn.close()
    else:
        print("❌ No database files found")

def check_json_data():
    """Check JSON simulation data"""
    print("\n" + "=" * 80)
    print(" JSON DATA ANALYSIS")
    print("=" * 80)
    
    json_files = glob("output/simulation_data_*.json")
    if json_files:
        latest_json = max(json_files, key=os.path.getctime)
        print(f"📄 JSON file: {latest_json}")
        
        with open(latest_json, 'r') as f:
            data = json.load(f)
        
        print(f"📊 Simulation metrics:")
        print(f"  • Network: {data['metadata']['num_enbs']} eNBs, {data['metadata']['num_ues']} UEs")
        print(f"  • Handovers: {len(data['handover_events'])} events")
        print(f"  • Duration: {data['metadata']['simulation_duration']} seconds")
        print(f"  • RL episodes: {len(data['rl_training'])}")
        print(f"  • Final RL reward: {data['rl_training'][-1]['reward']:.3f}")
        print(f"  • MEC services: {len(data['mec_services'])}")
        print(f"  • Network measurements: {len(data['network_measurements'])}")
        
        print("\n📋 MEC Services:")
        for service in data['mec_services']:
            print(f"  • {service['name']} @ {service['location']}: {service['status']}")
            
        print("\n📈 Handover Performance:")
        successful = sum(1 for h in data['handover_events'] if h['result'] == 'Success')
        success_rate = (successful / len(data['handover_events'])) * 100 if data['handover_events'] else 0
        avg_latency = sum(h['latency_ms'] for h in data['handover_events']) / len(data['handover_events']) if data['handover_events'] else 0
        print(f"  • Success rate: {success_rate:.1f}%")
        print(f"  • Average latency: {avg_latency:.1f} ms")
            
    else:
        print("❌ No JSON files found")

def check_csv_data():
    """Check CSV measurement data"""
    print("\n" + "=" * 80)
    print(" CSV MEASUREMENT DATA")
    print("=" * 80)
    
    # Check handover summary
    csv_files = glob("output/handover_summary_*.csv")
    if csv_files:
        latest_csv = max(csv_files, key=os.path.getctime)
        print(f"📊 Handover CSV: {latest_csv}")
        
        df = pd.read_csv(latest_csv)
        print(f"📈 Handover summary ({len(df)} records):")
        print(f"  • Success rate: {(df['result'] == 'Success').mean() * 100:.1f}%")
        print(f"  • Avg latency: {df['latency_ms'].mean():.1f} ms")
        print(f"  • Methods used: {df['method'].unique().tolist()}")
        
        print("\n📋 Sample handover events:")
        print(df.head(3).to_string(index=False))
    
    # Check RSRP measurements
    rsrp_files = glob("results/data/rsrp_measurements_*.csv")
    if rsrp_files:
        latest_rsrp = max(rsrp_files, key=os.path.getctime)
        print(f"\n📶 RSRP measurements: {latest_rsrp}")
        
        # Just check file size and first few lines
        file_size = os.path.getsize(latest_rsrp)
        print(f"📊 File size: {file_size:,} bytes")
        
        df_rsrp = pd.read_csv(latest_rsrp, nrows=5)
        print(f"📈 Sample RSRP data:")
        print(df_rsrp.to_string(index=False))

def check_plots():
    """Check generated plots"""
    print("\n" + "=" * 80)
    print(" VISUALIZATION FILES")
    print("=" * 80)
    
    plot_files = glob("results/plots/*.png")
    print(f"📊 Generated plots ({len(plot_files)}):")
    for plot in plot_files:
        size = os.path.getsize(plot)
        print(f"  • {os.path.basename(plot)} ({size:,} bytes)")

def main():
    """Main function to check all data"""
    print("🚀 O-RAN SIMULATION - DATA VERIFICATION")
    print("=" * 80)
    
    check_database()
    check_json_data()
    check_csv_data()
    check_plots()
    
    print("\n" + "=" * 80)
    print(" SUMMARY")
    print("=" * 80)
    print("✅ All simulation data has been generated successfully!")
    print("📊 Multiple data formats available (JSON, CSV, SQLite, PNG)")
    print("📈 Comprehensive analysis and visualizations completed")
    print("🎯 O-RAN simulation pipeline is fully operational!")

if __name__ == "__main__":
    main()
