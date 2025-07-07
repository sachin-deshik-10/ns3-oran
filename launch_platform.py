#!/usr/bin/env python3
"""
O-RAN 6G Platform Launcher
Easy launcher for different versions of the platform
"""

import subprocess
import sys
import os
import argparse

def main():
    parser = argparse.ArgumentParser(description='O-RAN 6G Platform Launcher')
    parser.add_argument('--app', choices=['basic', 'complete', 'enhancements'], 
                       default='basic',
                       help='Choose which app to launch (default: basic)')
    parser.add_argument('--port', type=int, default=8501,
                       help='Port number for Streamlit (default: 8501)')
    
    args = parser.parse_args()
    
    # Determine which app to launch
    app_files = {
        'basic': 'streamlit_oran_app.py',
        'complete': 'complete_streamlit_app.py', 
        'enhancements': 'streamlit_enhancements.py'
    }
    
    app_file = app_files[args.app]
    
    print(f"""
🚀 O-RAN 6G Platform Launcher
================================
Launching: {args.app.title()} Platform
File: {app_file}
Port: {args.port}
URL: http://localhost:{args.port}
================================
    """)
    
    # Change to the script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    
    try:
        # Launch Streamlit
        cmd = [sys.executable, '-m', 'streamlit', 'run', app_file, '--server.port', str(args.port)]
        subprocess.run(cmd)
    except KeyboardInterrupt:
        print("\n🛑 Shutting down O-RAN 6G Platform...")
    except Exception as e:
        print(f"❌ Error launching platform: {e}")

if __name__ == "__main__":
    main()
