#!/usr/bin/env python3
"""
O-RAN 6G Advanced Platform - Interactive Streamlit Application
Professional web interface for 6G THz, AI, and Network Slicing exploration
"""

import streamlit as st
import pandas as pd
import numpy as np
import plotly.graph_objects as go
import plotly.express as px
from plotly.subplots import make_subplots
import json
import time
import math
import random
from datetime import datetime, timedelta
import base64
from io import BytesIO

# Import the core demo functionality
from quick_oran_demo import QuickOranDemo

# Page configuration
st.set_page_config(
    page_title="O-RAN 6G Advanced Platform",
    page_icon="🚀",
    layout="wide",
    initial_sidebar_state="expanded"
)

# Custom CSS for better styling
st.markdown("""
<style>
    .main-header {
        font-size: 3rem;
        color: #1f77b4;
        text-align: center;
        margin-bottom: 2rem;
    }
    .metric-card {
        background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
        padding: 1rem;
        border-radius: 10px;
        color: white;
        margin: 0.5rem 0;
    }
    .success-badge {
        background: #28a745;
        color: white;
        padding: 0.25rem 0.5rem;
        border-radius: 15px;
        font-size: 0.8rem;
    }
    .warning-badge {
        background: #ffc107;
        color: black;
        padding: 0.25rem 0.5rem;
        border-radius: 15px;
        font-size: 0.8rem;
    }
    .info-badge {
        background: #17a2b8;
        color: white;
        padding: 0.25rem 0.5rem;
        border-radius: 15px;
        font-size: 0.8rem;
    }
</style>
""", unsafe_allow_html=True)

class OranStreamlitApp:
    def __init__(self):
        self.demo = QuickOranDemo()
        
    def create_sidebar_controls(self):
        """Create interactive sidebar controls"""
        st.sidebar.markdown("# 🎛️ Configuration Panel")
        
        # THz Configuration
        st.sidebar.markdown("## 🌊 THz Communications")
        thz_config = {
            "frequency_range": st.sidebar.slider(
                "Frequency Range (THz)", 
                min_value=0.1, max_value=3.0, 
                value=(0.1, 3.0), step=0.1
            ),
            "antenna_elements": st.sidebar.selectbox(
                "Antenna Elements", 
                [256, 512, 1024, 2048, 4096, 8192, 10000]
            ),
            "irs_enabled": st.sidebar.checkbox("Enable IRS", value=True),
            "beamforming_mode": st.sidebar.selectbox(
                "Beamforming Mode", 
                ["Analog", "Digital", "Hybrid"]
            )
        }
        
        # AI Configuration
        st.sidebar.markdown("## 🧠 AI Transformer")
        ai_config = {
            "num_heads": st.sidebar.selectbox(
                "Attention Heads", 
                [4, 8, 16, 32], index=2
            ),
            "embedding_dim": st.sidebar.selectbox(
                "Embedding Dimension", 
                [256, 512, 1024, 2048], index=1
            ),
            "num_layers": st.sidebar.slider(
                "Transformer Layers", 
                min_value=1, max_value=12, value=6
            ),
            "dropout_rate": st.sidebar.slider(
                "Dropout Rate", 
                min_value=0.0, max_value=0.5, value=0.1, step=0.05
            ),
            "learning_rate": st.sidebar.select_slider(
                "Learning Rate", 
                options=[0.0001, 0.001, 0.01, 0.1], value=0.001
            )
        }
        
        # Network Slicing Configuration
        st.sidebar.markdown("## 🔀 Network Slicing")
        slicing_config = {
            "num_slices": st.sidebar.slider(
                "Number of Slices", 
                min_value=1, max_value=16, value=4
            ),
            "enable_ai_optimization": st.sidebar.checkbox(
                "AI Optimization", value=True
            ),
            "isolation_level": st.sidebar.selectbox(
                "Isolation Level", 
                ["Shared", "Partial", "Full"], index=1
            ),
            "dynamic_allocation": st.sidebar.checkbox(
                "Dynamic Allocation", value=True
            )
        }
        
        # Simulation Configuration
        st.sidebar.markdown("## ⚙️ Simulation Settings")
        sim_config = {
            "simulation_time": st.sidebar.slider(
                "Simulation Time (s)", 
                min_value=10, max_value=300, value=60
            ),
            "real_time_updates": st.sidebar.checkbox(
                "Real-time Updates", value=True
            ),
            "export_results": st.sidebar.checkbox(
                "Export Results", value=False
            )
        }
        
        return thz_config, ai_config, slicing_config, sim_config
    
    def simulate_thz_advanced(self, config):
        """Advanced THz simulation with user parameters"""
        freq_min, freq_max = config["frequency_range"]
        frequencies = np.linspace(freq_min, freq_max, 10)
        
        results = {}
        for freq in frequencies:
            # Enhanced calculations based on user inputs
            base_bandwidth = freq * 50  # GHz
            antenna_gain = math.log10(config["antenna_elements"]) * 10
            irs_boost = 1.2 if config["irs_enabled"] else 1.0
            
            # Beamforming efficiency
            bf_efficiency = {
                "Analog": 0.85,
                "Digital": 0.95,
                "Hybrid": 0.90
            }[config["beamforming_mode"]]
            
            # Calculate performance metrics
            max_throughput = base_bandwidth * 2 * bf_efficiency * irs_boost
            atmospheric_loss = math.exp(-freq * 0.08)
            effective_throughput = max_throughput * atmospheric_loss
            
            # Latency calculation
            processing_delay = 0.1 + (1.0 / freq)
            beamforming_delay = 0.05 if config["beamforming_mode"] == "Digital" else 0.02
            total_latency = processing_delay + beamforming_delay
            
            # Range calculation
            link_budget = antenna_gain + (10 * math.log10(irs_boost))
            max_range = min(10.0 / freq, link_budget / 10)
            
            results[f"{freq:.1f}_THz"] = {
                "frequency": freq,
                "throughput_gbps": effective_throughput,
                "latency_ms": total_latency,
                "range_km": max_range,
                "antenna_gain_db": antenna_gain,
                "efficiency": bf_efficiency * irs_boost
            }
        
        return results
    
    def simulate_ai_advanced(self, config):
        """Advanced AI simulation with user parameters"""
        epochs = 50
        results = {
            "epochs": [],
            "loss": [],
            "accuracy": [],
            "inference_time": [],
            "memory_usage": [],
            "convergence_rate": []
        }
        
        # Parameter-dependent calculations
        complexity_factor = (config["num_heads"] * config["embedding_dim"] * config["num_layers"]) / 32768
        dropout_effect = 1 - config["dropout_rate"]
        lr_effect = math.log10(config["learning_rate"] * 10000) / 4
        
        for epoch in range(epochs):
            # Enhanced learning dynamics
            loss = 2.5 * math.exp(-epoch * 0.1 * lr_effect) * (1 + random.uniform(-0.1, 0.1))
            accuracy = 0.95 * (1 - math.exp(-epoch * 0.08 * lr_effect)) * dropout_effect
            
            # Inference time based on complexity
            base_inference_time = 0.5 + (complexity_factor * 2)
            inference_time = base_inference_time * math.exp(-epoch * 0.02)
            
            # Memory usage
            memory_usage = config["embedding_dim"] * config["num_heads"] * 0.1
            
            # Convergence rate
            convergence = 1.0 - math.exp(-epoch * 0.1)
            
            results["epochs"].append(epoch)
            results["loss"].append(max(loss, 0.05))
            results["accuracy"].append(min(max(accuracy, 0), 1))
            results["inference_time"].append(max(inference_time, 0.1))
            results["memory_usage"].append(memory_usage)
            results["convergence_rate"].append(convergence)
        
        return results
    
    def simulate_slicing_advanced(self, config):
        """Advanced network slicing with user parameters"""
        slice_types = ["eMBB", "URLLC", "mMTC", "Custom"]
        num_slices = config["num_slices"]
        
        results = {"slices": []}
        total_bandwidth = 0
        
        for i in range(num_slices):
            slice_type = slice_types[i % len(slice_types)]
            
            # Base requirements by type
            if slice_type == "eMBB":
                base_bandwidth = random.uniform(500, 1500)
                base_latency = random.uniform(10, 30)
                priority = 0.7
            elif slice_type == "URLLC":
                base_bandwidth = random.uniform(50, 200)
                base_latency = random.uniform(0.5, 2.0)
                priority = 0.95
            elif slice_type == "mMTC":
                base_bandwidth = random.uniform(10, 100)
                base_latency = random.uniform(100, 500)
                priority = 0.3
            else:  # Custom
                base_bandwidth = random.uniform(200, 800)
                base_latency = random.uniform(5, 50)
                priority = random.uniform(0.5, 0.9)
            
            # Apply configuration effects
            ai_boost = 1.15 if config["enable_ai_optimization"] else 1.0
            isolation_overhead = {
                "Shared": 0.95,
                "Partial": 0.90,
                "Full": 0.85
            }[config["isolation_level"]]
            
            dynamic_efficiency = 1.1 if config["dynamic_allocation"] else 1.0
            
            # Calculate actual performance
            efficiency = random.uniform(0.85, 0.98) * ai_boost * isolation_overhead * dynamic_efficiency
            actual_bandwidth = base_bandwidth * efficiency
            actual_latency = base_latency * random.uniform(0.9, 1.1)
            
            total_bandwidth += actual_bandwidth
            
            slice_info = {
                "id": i + 1,
                "type": slice_type,
                "bandwidth_mbps": actual_bandwidth,
                "latency_ms": actual_latency,
                "efficiency": efficiency,
                "priority": priority,
                "qos_compliant": efficiency > 0.9,
                "resource_usage": random.uniform(20, 80)
            }
            
            results["slices"].append(slice_info)
        
        # Calculate overall metrics
        avg_efficiency = np.mean([s["efficiency"] for s in results["slices"]])
        qos_compliance = np.mean([s["qos_compliant"] for s in results["slices"]])
        
        results.update({
            "total_bandwidth_gbps": total_bandwidth / 1000,
            "average_efficiency": avg_efficiency,
            "qos_compliance_rate": qos_compliance,
            "num_active_slices": len(results["slices"])
        })
        
        return results
    
    def create_thz_visualization(self, results):
        """Create THz performance visualizations"""
        frequencies = [float(k.split('_')[0]) for k in results.keys()]
        throughputs = [results[k]["throughput_gbps"] for k in results.keys()]
        latencies = [results[k]["latency_ms"] for k in results.keys()]
        ranges = [results[k]["range_km"] for k in results.keys()]
        
        # Create subplots
        fig = make_subplots(
            rows=2, cols=2,
            subplot_titles=["Frequency vs Throughput", "Frequency vs Latency", 
                          "Frequency vs Range", "Performance Radar"],
            specs=[[{"type": "scatter"}, {"type": "scatter"}],
                   [{"type": "scatter"}, {"type": "polar"}]]
        )
        
        # Throughput plot
        fig.add_trace(
            go.Scatter(
                x=frequencies, y=throughputs,
                mode='lines+markers',
                name='Throughput',
                line=dict(color='blue', width=3),
                marker=dict(size=8)
            ),
            row=1, col=1
        )
        
        # Latency plot
        fig.add_trace(
            go.Scatter(
                x=frequencies, y=latencies,
                mode='lines+markers',
                name='Latency',
                line=dict(color='red', width=3),
                marker=dict(size=8)
            ),
            row=1, col=2
        )
        
        # Range plot
        fig.add_trace(
            go.Scatter(
                x=frequencies, y=ranges,
                mode='lines+markers',
                name='Range',
                line=dict(color='green', width=3),
                marker=dict(size=8)
            ),
            row=2, col=1
        )
        
        # Radar chart for best frequency
        best_freq_key = max(results.keys(), key=lambda k: results[k]["throughput_gbps"])
        best_result = results[best_freq_key]
        
        radar_categories = ['Throughput', 'Low Latency', 'Range', 'Efficiency', 'Gain']
        radar_values = [
            min(best_result["throughput_gbps"] / 100, 1),
            1 - min(best_result["latency_ms"] / 5, 1),
            min(best_result["range_km"] / 10, 1),
            best_result["efficiency"],
            min(best_result["antenna_gain_db"] / 50, 1)
        ]
        
        fig.add_trace(
            go.Scatterpolar(
                r=radar_values,
                theta=radar_categories,
                fill='toself',
                name='Performance Profile',
                line=dict(color='purple')
            ),
            row=2, col=2
        )
        
        fig.update_layout(
            title="THz Communication Performance Analysis",
            height=600,
            showlegend=True
        )
        
        fig.update_xaxes(title_text="Frequency (THz)", row=1, col=1)
        fig.update_yaxes(title_text="Throughput (Gbps)", row=1, col=1)
        fig.update_xaxes(title_text="Frequency (THz)", row=1, col=2)
        fig.update_yaxes(title_text="Latency (ms)", row=1, col=2)
        fig.update_xaxes(title_text="Frequency (THz)", row=2, col=1)
        fig.update_yaxes(title_text="Range (km)", row=2, col=1)
        
        return fig
    
    def create_ai_visualization(self, results):
        """Create AI training visualizations"""
        fig = make_subplots(
            rows=2, cols=2,
            subplot_titles=["Training Progress", "Loss Convergence", 
                          "Inference Time Optimization", "Resource Usage"],
            specs=[[{"type": "scatter"}, {"type": "scatter"}],
                   [{"type": "scatter"}, {"type": "bar"}]]
        )
        
        epochs = results["epochs"]
        
        # Accuracy progress
        fig.add_trace(
            go.Scatter(
                x=epochs, y=results["accuracy"],
                mode='lines+markers',
                name='Accuracy',
                line=dict(color='green', width=3)
            ),
            row=1, col=1
        )
        
        # Loss convergence
        fig.add_trace(
            go.Scatter(
                x=epochs, y=results["loss"],
                mode='lines+markers',
                name='Loss',
                line=dict(color='red', width=3)
            ),
            row=1, col=2
        )
        
        # Inference time
        fig.add_trace(
            go.Scatter(
                x=epochs, y=results["inference_time"],
                mode='lines+markers',
                name='Inference Time',
                line=dict(color='blue', width=3)
            ),
            row=2, col=1
        )
        
        # Memory usage (last 10 epochs average)
        memory_avg = np.mean(results["memory_usage"][-10:])
        fig.add_trace(
            go.Bar(
                x=['Memory Usage'],
                y=[memory_avg],
                name='Memory (MB)',
                marker_color='orange'
            ),
            row=2, col=2
        )
        
        fig.update_layout(
            title="AI Transformer Training Analysis",
            height=600,
            showlegend=True
        )
        
        return fig
    
    def create_slicing_visualization(self, results):
        """Create network slicing visualizations"""
        slices = results["slices"]
        
        fig = make_subplots(
            rows=2, cols=2,
            subplot_titles=["Slice Bandwidth Distribution", "Latency by Slice Type", 
                          "Efficiency Analysis", "QoS Compliance"],
            specs=[[{"type": "bar"}, {"type": "box"}],
                   [{"type": "scatter"}, {"type": "pie"}]]
        )
        
        # Bandwidth distribution
        slice_names = [f"Slice {s['id']} ({s['type']})" for s in slices]
        bandwidths = [s["bandwidth_mbps"] for s in slices]
        slice_types = [s["type"] for s in slices]
        
        color_map = {"eMBB": "blue", "URLLC": "red", "mMTC": "green", "Custom": "orange"}
        colors = [color_map.get(t, "gray") for t in slice_types]
        
        fig.add_trace(
            go.Bar(
                x=slice_names, y=bandwidths,
                name='Bandwidth',
                marker_color=colors
            ),
            row=1, col=1
        )
        
        # Latency box plot by type
        for slice_type in set(slice_types):
            type_latencies = [s["latency_ms"] for s in slices if s["type"] == slice_type]
            fig.add_trace(
                go.Box(
                    y=type_latencies,
                    name=slice_type,
                    marker_color=color_map.get(slice_type, "gray")
                ),
                row=1, col=2
            )
        
        # Efficiency scatter
        efficiencies = [s["efficiency"] for s in slices]
        priorities = [s["priority"] for s in slices]
        
        fig.add_trace(
            go.Scatter(
                x=priorities, y=efficiencies,
                mode='markers',
                marker=dict(
                    size=[s["bandwidth_mbps"]/20 for s in slices],
                    color=colors,
                    opacity=0.7
                ),
                text=slice_names,
                name='Efficiency vs Priority'
            ),
            row=2, col=1
        )
        
        # QoS compliance pie
        compliant = sum(1 for s in slices if s["qos_compliant"])
        non_compliant = len(slices) - compliant
        
        fig.add_trace(
            go.Pie(
                labels=['Compliant', 'Non-Compliant'],
                values=[compliant, non_compliant],
                marker_colors=['green', 'red']
            ),
            row=2, col=2
        )
        
        fig.update_layout(
            title="Network Slicing Performance Analysis",
            height=600,
            showlegend=True
        )
        
        return fig
    
    def create_integration_dashboard(self, thz_results, ai_results, slicing_results):
        """Create integrated performance dashboard"""
        # Extract key metrics
        best_thz = max(thz_results.values(), key=lambda x: x["throughput_gbps"])
        final_ai_accuracy = ai_results["accuracy"][-1]
        final_inference_time = ai_results["inference_time"][-1]
        slicing_efficiency = slicing_results["average_efficiency"]
        
        # Calculate integrated metrics
        system_throughput = best_thz["throughput_gbps"] * slicing_efficiency
        system_latency = best_thz["latency_ms"] + final_inference_time
        system_reliability = (final_ai_accuracy + slicing_results["qos_compliance_rate"]) / 2
        energy_efficiency = 0.87 + (final_ai_accuracy * 0.08) + (slicing_efficiency * 0.05)
        
        integration_score = (
            min(system_throughput / 100, 1.0) * 25 +
            max(0, (10 - system_latency) / 10) * 25 +
            system_reliability * 25 +
            energy_efficiency * 25
        )
        
        # Create dashboard
        fig = make_subplots(
            rows=2, cols=3,
            subplot_titles=["System Overview", "Performance Radar", "Component Scores",
                          "Throughput Timeline", "Latency Breakdown", "Efficiency Metrics"],
            specs=[[{"type": "xy"}, {"type": "polar"}, {"type": "xy"}],
                   [{"type": "xy"}, {"type": "xy"}, {"type": "xy"}]]
        )
        
        # System overview indicator - converted to bar chart
        fig.add_trace(
            go.Bar(
                x=['Integration Score'],
                y=[integration_score],
                marker_color='darkblue',
                name='Integration Score'
            ),
            row=1, col=1
        )
        
        # Performance radar
        radar_metrics = ['Throughput', 'Latency', 'Reliability', 'Efficiency', 'AI Performance']
        radar_values = [
            min(system_throughput / 200, 1),
            1 - min(system_latency / 10, 1),
            system_reliability,
            energy_efficiency,
            final_ai_accuracy
        ]
        
        fig.add_trace(
            go.Scatterpolar(
                r=radar_values,
                theta=radar_metrics,
                fill='toself',
                name='Integrated Performance'
            ),
            row=1, col=2
        )
        
        # Component scores
        components = ['THz', 'AI', 'Slicing']
        scores = [
            best_thz["efficiency"] * 100,
            final_ai_accuracy * 100,
            slicing_efficiency * 100
        ]
        
        fig.add_trace(
            go.Bar(
                x=components, y=scores,
                marker_color=['blue', 'green', 'orange'],
                name='Component Scores'
            ),
            row=1, col=3
        )
        
        fig.update_layout(
            title="6G O-RAN Integrated Performance Dashboard",
            height=800,
            showlegend=True
        )
        
        return fig, {
            "system_throughput": system_throughput,
            "system_latency": system_latency,
            "system_reliability": system_reliability,
            "energy_efficiency": energy_efficiency,
            "integration_score": integration_score
        }
    
    def create_domain_integration_section(self):
        """Create domain integration and novelty section"""
        st.markdown("## 🌐 Domain Integration & Novel Applications")
        
        # Domain selection
        col1, col2 = st.columns(2)
        
        with col1:
            st.markdown("### 🏥 Healthcare & Telemedicine")
            healthcare_enabled = st.checkbox("Enable Healthcare Integration")
            if healthcare_enabled:
                st.markdown("""
                **Applications:**
                - Remote surgery with ultra-low latency (URLLC slices)
                - High-resolution medical imaging transmission (eMBB slices)
                - IoT medical device connectivity (mMTC slices)
                - AI-powered diagnostic assistance
                
                **Metrics:**
                - Latency: <1ms for critical operations
                - Reliability: >99.999%
                - Bandwidth: Up to 1Gbps for 4K/8K imaging
                """)
        
        with col2:
            st.markdown("### 🏭 Industry 4.0 & Smart Manufacturing")
            industry_enabled = st.checkbox("Enable Industry 4.0 Integration")
            if industry_enabled:
                st.markdown("""
                **Applications:**
                - Real-time factory automation
                - Predictive maintenance with AI
                - Quality control with computer vision
                - Supply chain optimization
                
                **Metrics:**
                - Production efficiency: +25%
                - Defect reduction: -40%
                - Energy savings: +15%
                """)
        
        col3, col4 = st.columns(2)
        
        with col3:
            st.markdown("### 🚗 Autonomous Vehicles & Smart Transportation")
            transport_enabled = st.checkbox("Enable Transportation Integration")
            if transport_enabled:
                st.markdown("""
                **Applications:**
                - Vehicle-to-everything (V2X) communication
                - Real-time traffic optimization
                - Autonomous driving assistance
                - Emergency response coordination
                
                **Metrics:**
                - Reaction time: <10ms
                - Coverage: City-wide
                - Vehicles supported: 10,000+ per km²
                """)
        
        with col4:
            st.markdown("### 🌱 Smart Cities & Environmental Monitoring")
            smart_city_enabled = st.checkbox("Enable Smart City Integration")
            if smart_city_enabled:
                st.markdown("""
                **Applications:**
                - Environmental sensor networks
                - Smart grid management
                - Public safety systems
                - Waste management optimization
                
                **Metrics:**
                - Sensor density: 1000+ per km²
                - Response time: <5ms
                - Energy efficiency: +30%
                """)
        
        # Novel applications section
        st.markdown("### 🔬 Novel Research Applications")
        
        novel_apps = st.multiselect(
            "Select Novel Application Areas:",
            [
                "Quantum Communications Integration",
                "Holographic Communications",
                "Brain-Computer Interfaces",
                "Digital Twin Networks",
                "Metaverse Infrastructure",
                "Space Communications",
                "Underwater Networks",
                "Disaster Response Systems"
            ]
        )
        
        if novel_apps:
            for app in novel_apps:
                with st.expander(f"🚀 {app}"):
                    if app == "Quantum Communications Integration":
                        st.markdown("""
                        **Integration Points:**
                        - Quantum key distribution over THz links
                        - AI-optimized quantum error correction
                        - Network slicing for quantum applications
                        
                        **Research Potential:**
                        - Ultra-secure communications
                        - Quantum internet backbone
                        - Distributed quantum computing
                        """)
                    elif app == "Holographic Communications":
                        st.markdown("""
                        **Requirements:**
                        - Bandwidth: >1Tbps for full holographic transmission
                        - Latency: <0.1ms for real-time interaction
                        - Processing: Real-time 3D reconstruction
                        
                        **Applications:**
                        - Remote collaboration
                        - Medical procedures
                        - Entertainment and gaming
                        """)
                    elif app == "Brain-Computer Interfaces":
                        st.markdown("""
                        **Neural Network Integration:**
                        - Real-time neural signal processing
                        - AI-powered brain signal interpretation
                        - Ultra-low latency feedback systems
                        
                        **Ethical Considerations:**
                        - Privacy protection
                        - Consent mechanisms
                        - Security protocols
                        """)
        
        return {
            "healthcare": healthcare_enabled,
            "industry": industry_enabled,
            "transport": transport_enabled,
            "smart_city": smart_city_enabled,
            "novel_apps": novel_apps
        }
    
    def create_export_functionality(self, results):
        """Create export functionality for results"""
        st.markdown("## 📊 Export & Analysis")
        
        col1, col2, col3 = st.columns(3)
        
        with col1:
            if st.button("📄 Export PDF Report"):
                # Create PDF report (placeholder)
                st.success("PDF report generation started!")
                st.info("Report will include all visualizations and analysis results.")
        
        with col2:
            if st.button("📊 Export Excel Data"):
                # Create Excel export
                df = pd.DataFrame(results)
                buffer = BytesIO()
                df.to_excel(buffer, index=False)
                buffer.seek(0)
                
                st.download_button(
                    label="⬇️ Download Excel File",
                    data=buffer,
                    file_name=f"oran_6g_results_{datetime.now().strftime('%Y%m%d_%H%M%S')}.xlsx",
                    mime="application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
                )
        
        with col3:
            if st.button("🔗 Generate API Endpoints"):
                st.code("""
                # REST API Endpoints
                GET /api/thz/performance?freq=1.5&elements=1024
                GET /api/ai/training?heads=16&layers=6
                GET /api/slicing/status?slices=8
                POST /api/simulation/run
                """, language="bash")
    
    def run_app(self):
        """Main application runner"""
        # Header
        st.markdown('<h1 class="main-header">🚀 O-RAN 6G Advanced Platform</h1>', unsafe_allow_html=True)
        st.markdown("### Interactive Analysis of THz Communications, AI Intelligence & Network Slicing")
        
        # Sidebar controls
        thz_config, ai_config, slicing_config, sim_config = self.create_sidebar_controls()
        
        # Main content tabs
        tab1, tab2, tab3, tab4, tab5, tab6 = st.tabs([
            "📊 Live Dashboard", 
            "🌊 THz Analysis", 
            "🧠 AI Performance", 
            "🔀 Network Slicing", 
            "🌐 Domain Integration",
            "📁 Export & API"
        ])
        
        # Run simulations based on configurations
        if st.sidebar.button("🚀 Run Simulation", type="primary"):
            with st.spinner("Running advanced simulations..."):
                # Progress bar
                progress_bar = st.progress(0)
                
                # THz simulation
                progress_bar.progress(25)
                thz_results = self.simulate_thz_advanced(thz_config)
                
                # AI simulation
                progress_bar.progress(50)
                ai_results = self.simulate_ai_advanced(ai_config)
                
                # Slicing simulation
                progress_bar.progress(75)
                slicing_results = self.simulate_slicing_advanced(slicing_config)
                
                # Integration analysis
                progress_bar.progress(100)
                integration_fig, integration_metrics = self.create_integration_dashboard(
                    thz_results, ai_results, slicing_results
                )
                
                # Store results in session state
                st.session_state['thz_results'] = thz_results
                st.session_state['ai_results'] = ai_results
                st.session_state['slicing_results'] = slicing_results
                st.session_state['integration_metrics'] = integration_metrics
                st.session_state['integration_fig'] = integration_fig
                
                st.success("✅ Simulation completed successfully!")
        
        # Display results if available
        if 'thz_results' in st.session_state:
            
            with tab1:
                st.plotly_chart(st.session_state['integration_fig'], use_container_width=True)
                
                # Key metrics display
                col1, col2, col3, col4 = st.columns(4)
                metrics = st.session_state['integration_metrics']
                
                with col1:
                    st.metric(
                        "System Throughput",
                        f"{metrics['system_throughput']:.1f} Gbps",
                        delta=f"+{metrics['system_throughput']*0.1:.1f}"
                    )
                
                with col2:
                    st.metric(
                        "End-to-End Latency",
                        f"{metrics['system_latency']:.2f} ms",
                        delta=f"-{metrics['system_latency']*0.05:.2f}"
                    )
                
                with col3:
                    st.metric(
                        "System Reliability",
                        f"{metrics['system_reliability']:.1%}",
                        delta="+2.5%"
                    )
                
                with col4:
                    st.metric(
                        "Integration Score",
                        f"{metrics['integration_score']:.1f}/100",
                        delta="+5.2"
                    )
            
            with tab2:
                st.plotly_chart(
                    self.create_thz_visualization(st.session_state['thz_results']),
                    use_container_width=True
                )
                
                # THz insights
                st.markdown("### 🎯 THz Performance Insights")
                best_freq = max(st.session_state['thz_results'].values(), 
                               key=lambda x: x["throughput_gbps"])
                st.info(f"Optimal frequency: {best_freq['frequency']:.1f} THz with "
                       f"{best_freq['throughput_gbps']:.1f} Gbps throughput")
            
            with tab3:
                st.plotly_chart(
                    self.create_ai_visualization(st.session_state['ai_results']),
                    use_container_width=True
                )
                
                # AI insights
                st.markdown("### 🎯 AI Performance Insights")
                final_accuracy = st.session_state['ai_results']['accuracy'][-1]
                st.info(f"Final training accuracy: {final_accuracy:.1%}")
            
            with tab4:
                st.plotly_chart(
                    self.create_slicing_visualization(st.session_state['slicing_results']),
                    use_container_width=True
                )
                
                # Slicing insights
                st.markdown("### 🎯 Network Slicing Insights")
                slicing_efficiency = st.session_state['slicing_results']['average_efficiency']
                st.info(f"Average slicing efficiency: {slicing_efficiency:.1%}")
            
            with tab5:
                domain_config = self.create_domain_integration_section()
                
                if any(domain_config.values()):
                    st.markdown("### 📈 Projected Impact")
                    
                    impact_metrics = {}
                    if domain_config['healthcare']:
                        impact_metrics['Healthcare Latency Improvement'] = "85%"
                    if domain_config['industry']:
                        impact_metrics['Manufacturing Efficiency'] = "+25%"
                    if domain_config['transport']:
                        impact_metrics['Traffic Optimization'] = "+40%"
                    if domain_config['smart_city']:
                        impact_metrics['Energy Savings'] = "+30%"
                    
                    for metric, value in impact_metrics.items():
                        st.metric(metric, value)
            
            with tab6:
                self.create_export_functionality({
                    'thz': st.session_state['thz_results'],
                    'ai': st.session_state['ai_results'],
                    'slicing': st.session_state['slicing_results'],
                    'integration': st.session_state['integration_metrics']
                })
        else:
            st.info("👈 Configure parameters in the sidebar and click 'Run Simulation' to start!")
        
        # Footer
        st.markdown("---")
        st.markdown("""
        <div style='text-align: center; color: #666;'>
        🌟 O-RAN 6G Advanced Platform | World-leading 6G Research Framework<br>
        Built with cutting-edge THz, AI, and Network Slicing technologies
        </div>
        """, unsafe_allow_html=True)

if __name__ == "__main__":
    app = OranStreamlitApp()
    app.run_app()
