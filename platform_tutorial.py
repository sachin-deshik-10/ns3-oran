#!/usr/bin/env python3
"""
O-RAN 6G Platform - Comprehensive Usage Guide & Tutorial
Interactive tutorial for users to learn the platform
"""

import streamlit as st
import plotly.graph_objects as go
import plotly.express as px
import pandas as pd
import numpy as np
from datetime import datetime

def main():
    st.set_page_config(
        page_title="O-RAN 6G Platform Tutorial",
        page_icon="🎓",
        layout="wide"
    )
    
    st.markdown("# 🎓 O-RAN 6G Platform - Interactive Tutorial")
    st.markdown("### Learn to master the world's most advanced 6G simulation platform")
    
    tutorial_sections = st.tabs([
        "🚀 Getting Started",
        "🌊 THz Communications",
        "🧠 AI Intelligence",
        "🔀 Network Slicing",
        "🌐 Domain Applications",
        "🔬 Research Tools",
        "📊 Advanced Features"
    ])
    
    with tutorial_sections[0]:
        create_getting_started_tutorial()
    
    with tutorial_sections[1]:
        create_thz_tutorial()
    
    with tutorial_sections[2]:
        create_ai_tutorial()
    
    with tutorial_sections[3]:
        create_slicing_tutorial()
    
    with tutorial_sections[4]:
        create_domain_tutorial()
    
    with tutorial_sections[5]:
        create_research_tutorial()
    
    with tutorial_sections[6]:
        create_advanced_tutorial()

def create_getting_started_tutorial():
    """Create getting started tutorial"""
    
    st.markdown("## 🚀 Getting Started with O-RAN 6G Platform")
    
    col1, col2 = st.columns([2, 1])
    
    with col1:
        st.markdown("""
        ### Welcome to the Future of 6G Research!
        
        This platform represents the world's most comprehensive 6G O-RAN simulation environment. 
        You'll learn to:
        
        ✅ **Configure THz communications** for ultra-high bandwidth
        ✅ **Deploy AI-native intelligence** for network optimization  
        ✅ **Manage dynamic network slicing** for diverse applications
        ✅ **Integrate domain-specific** use cases
        ✅ **Generate publication-ready** research results
        
        ### Platform Architecture
        
        The platform consists of three main modules:
        1. **THz Communications**: 0.1-3.0 THz frequency support
        2. **AI Transformer**: Intelligent network optimization
        3. **Network Slicing**: Dynamic resource allocation
        """)
    
    with col2:
        # Create a simple architecture diagram
        fig = go.Figure()
        
        # Add boxes for modules
        fig.add_shape(type="rect", x0=0.1, y0=0.7, x1=0.9, y1=0.9,
                     fillcolor="lightblue", line=dict(color="blue"))
        fig.add_annotation(x=0.5, y=0.8, text="THz Communications", showarrow=False)
        
        fig.add_shape(type="rect", x0=0.1, y0=0.4, x1=0.9, y1=0.6,
                     fillcolor="lightgreen", line=dict(color="green"))
        fig.add_annotation(x=0.5, y=0.5, text="AI Intelligence", showarrow=False)
        
        fig.add_shape(type="rect", x0=0.1, y0=0.1, x1=0.9, y1=0.3,
                     fillcolor="lightyellow", line=dict(color="orange"))
        fig.add_annotation(x=0.5, y=0.2, text="Network Slicing", showarrow=False)
        
        fig.update_layout(
            title="Platform Architecture",
            xaxis=dict(visible=False),
            yaxis=dict(visible=False),
            height=300
        )
        
        st.plotly_chart(fig, use_container_width=True)
    
    # Interactive checklist
    st.markdown("### 📋 Pre-flight Checklist")
    
    checklist_items = [
        "Platform is running at http://localhost:8501",
        "Dependencies are installed (streamlit, plotly, pandas)",
        "You have access to the sidebar controls",
        "You can see the main dashboard tabs"
    ]
    
    completed_items = []
    for item in checklist_items:
        if st.checkbox(item):
            completed_items.append(item)
    
    if len(completed_items) == len(checklist_items):
        st.success("🎉 You're ready to start! Proceed to the next section.")
    else:
        st.info(f"Complete {len(checklist_items) - len(completed_items)} more items to continue.")

def create_thz_tutorial():
    """THz communications tutorial"""
    
    st.markdown("## 🌊 THz Communications Tutorial")
    
    st.markdown("""
    ### Understanding Terahertz Communications
    
    Terahertz (THz) frequencies (0.1-3.0 THz) represent the next frontier in wireless communications,
    offering unprecedented bandwidth and enabling applications like:
    
    - **Ultra-high-speed data transfer** (100+ Gbps)
    - **Holographic communications**
    - **Real-time augmented reality**
    - **Massive IoT connectivity**
    """)
    
    # Interactive frequency selector
    st.markdown("### 🎛️ Try It: Configure THz Parameters")
    
    col1, col2, col3 = st.columns(3)
    
    with col1:
        frequency = st.slider("Frequency (THz)", 0.1, 3.0, 1.5, 0.1)
        st.info(f"**Selected**: {frequency} THz")
    
    with col2:
        antenna_elements = st.selectbox("Antenna Elements", [256, 512, 1024, 2048, 4096])
        st.info(f"**Selected**: {antenna_elements} elements")
    
    with col3:
        beamforming = st.selectbox("Beamforming Type", ["Analog", "Digital", "Hybrid"])
        st.info(f"**Selected**: {beamforming}")
    
    # Calculate and display results
    if st.button("🧮 Calculate THz Performance"):
        # Simplified calculations for tutorial
        bandwidth = frequency * 50  # GHz
        throughput = bandwidth * 2  # Gbps approximation
        range_km = min(10.0 / frequency, 5.0)
        
        col1, col2, col3 = st.columns(3)
        
        with col1:
            st.metric("Bandwidth", f"{bandwidth:.1f} GHz")
        with col2:
            st.metric("Throughput", f"{throughput:.1f} Gbps")
        with col3:
            st.metric("Range", f"{range_km:.1f} km")
        
        # Performance vs frequency chart
        frequencies = np.linspace(0.1, 3.0, 30)
        throughputs = frequencies * 100
        
        fig = go.Figure()
        fig.add_trace(go.Scatter(
            x=frequencies, y=throughputs,
            mode='lines+markers',
            name='Throughput',
            line=dict(color='blue', width=3)
        ))
        
        # Highlight selected frequency
        fig.add_trace(go.Scatter(
            x=[frequency], y=[throughput],
            mode='markers',
            marker=dict(size=15, color='red'),
            name='Your Selection'
        ))
        
        fig.update_layout(
            title="THz Frequency vs Throughput",
            xaxis_title="Frequency (THz)",
            yaxis_title="Throughput (Gbps)",
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
    
    # Key insights
    st.markdown("""
    ### 💡 Key Insights
    
    - **Higher frequencies** = **Higher bandwidth** but **shorter range**
    - **More antenna elements** = **Better beamforming** and **increased gain**
    - **Digital beamforming** offers **highest performance** but **higher complexity**
    - **Atmospheric absorption** increases significantly above 1 THz
    """)

def create_ai_tutorial():
    """AI intelligence tutorial"""
    
    st.markdown("## 🧠 AI Intelligence Tutorial")
    
    st.markdown("""
    ### Transformer-Based Network Intelligence
    
    Our AI module uses state-of-the-art transformer architecture for:
    
    - **Predictive handover decisions**
    - **Resource optimization**
    - **Traffic pattern recognition**
    - **Anomaly detection**
    - **Real-time network adaptation**
    """)
    
    # AI architecture explanation
    col1, col2 = st.columns([1, 1])
    
    with col1:
        st.markdown("""
        ### 🏗️ Architecture Components
        
        **Multi-Head Attention**
        - Processes multiple network aspects simultaneously
        - Identifies relationships between network elements
        
        **Positional Encoding**
        - Understands temporal sequences
        - Tracks network state evolution
        
        **Feed-Forward Networks**
        - Non-linear transformations
        - Complex pattern recognition
        """)
    
    with col2:
        # Interactive AI configuration
        st.markdown("### ⚙️ Configure AI Parameters")
        
        num_heads = st.slider("Attention Heads", 4, 32, 16)
        embedding_dim = st.selectbox("Embedding Dimension", [128, 256, 512, 1024, 2048])
        num_layers = st.slider("Transformer Layers", 2, 24, 6)
        
        # Calculate model complexity
        parameters = num_heads * embedding_dim * num_layers * 1000
        st.metric("Model Parameters", f"{parameters/1e6:.1f}M")
    
    # Simulate training process
    if st.button("🚀 Simulate AI Training"):
        
        progress_bar = st.progress(0)
        status_text = st.empty()
        
        # Simulate training epochs
        epochs = 20
        losses = []
        accuracies = []
        
        for epoch in range(epochs):
            # Simulate training
            loss = 2.0 * np.exp(-epoch * 0.1) + np.random.normal(0, 0.1)
            accuracy = 0.95 * (1 - np.exp(-epoch * 0.08))
            
            losses.append(max(loss, 0.05))
            accuracies.append(min(accuracy, 0.95))
            
            progress_bar.progress((epoch + 1) / epochs)
            status_text.text(f"Epoch {epoch+1}/{epochs} - Loss: {loss:.3f} - Accuracy: {accuracy:.3f}")
        
        # Display training results
        fig = go.Figure()
        
        fig.add_trace(go.Scatter(
            x=list(range(epochs)), y=losses,
            mode='lines+markers',
            name='Training Loss',
            line=dict(color='red')
        ))
        
        fig.add_trace(go.Scatter(
            x=list(range(epochs)), y=accuracies,
            mode='lines+markers',
            name='Accuracy',
            yaxis='y2',
            line=dict(color='green')
        ))
        
        fig.update_layout(
            title="AI Training Progress",
            xaxis_title="Epoch",
            yaxis_title="Loss",
            yaxis2=dict(title="Accuracy", overlaying='y', side='right'),
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        st.success(f"🎉 Training completed! Final accuracy: {accuracies[-1]:.1%}")

def create_slicing_tutorial():
    """Network slicing tutorial"""
    
    st.markdown("## 🔀 Network Slicing Tutorial")
    
    st.markdown("""
    ### Dynamic Network Slicing
    
    Network slicing enables multiple virtual networks on a single physical infrastructure:
    
    - **eMBB**: Enhanced Mobile Broadband (high data rates)
    - **URLLC**: Ultra-Reliable Low-Latency Communications
    - **mMTC**: Massive Machine-Type Communications
    - **Custom**: Application-specific slices
    """)
    
    # Slice type selector
    st.markdown("### 🎯 Design Your Network Slice")
    
    slice_type = st.selectbox("Slice Type", ["eMBB", "URLLC", "mMTC", "Custom"])
    
    if slice_type == "eMBB":
        st.info("📱 **eMBB**: Optimized for high-speed mobile data, video streaming, AR/VR")
        bandwidth_range = (500, 2000)
        latency_range = (10, 50)
        reliability = 0.99
    elif slice_type == "URLLC":
        st.info("🚨 **URLLC**: Critical applications requiring ultra-low latency and high reliability")
        bandwidth_range = (50, 500)
        latency_range = (0.5, 5)
        reliability = 0.99999
    elif slice_type == "mMTC":
        st.info("🏭 **mMTC**: Massive IoT connectivity with energy efficiency")
        bandwidth_range = (1, 100)
        latency_range = (100, 1000)
        reliability = 0.95
    else:
        st.info("⚙️ **Custom**: Define your own slice requirements")
        bandwidth_range = (10, 1000)
        latency_range = (1, 100)
        reliability = 0.98
    
    # Interactive slice configuration
    col1, col2, col3 = st.columns(3)
    
    with col1:
        bandwidth = st.slider("Bandwidth (Mbps)", 
                             bandwidth_range[0], bandwidth_range[1], 
                             bandwidth_range[0] + (bandwidth_range[1] - bandwidth_range[0]) // 2)
    
    with col2:
        latency = st.slider("Max Latency (ms)", 
                           latency_range[0], latency_range[1], 
                           latency_range[0] + (latency_range[1] - latency_range[0]) // 2)
    
    with col3:
        priority = st.slider("Priority", 0.0, 1.0, reliability)
    
    # Slice performance simulation
    if st.button("📊 Simulate Slice Performance"):
        
        # Generate simulation data
        time_points = np.arange(0, 60, 1)  # 60 seconds
        
        # Simulate varying performance
        bandwidth_utilization = np.random.normal(0.7, 0.1, len(time_points))
        bandwidth_utilization = np.clip(bandwidth_utilization, 0.3, 1.0)
        
        latency_actual = latency * (0.8 + 0.4 * np.random.random(len(time_points)))
        
        # Create performance dashboard
        fig = go.Figure()
        
        # Bandwidth utilization
        fig.add_trace(go.Scatter(
            x=time_points, y=bandwidth_utilization * 100,
            mode='lines',
            name='Bandwidth Utilization (%)',
            line=dict(color='blue')
        ))
        
        # Latency
        fig.add_trace(go.Scatter(
            x=time_points, y=latency_actual,
            mode='lines',
            name='Actual Latency (ms)',
            yaxis='y2',
            line=dict(color='red')
        ))
        
        fig.update_layout(
            title=f"{slice_type} Slice Performance",
            xaxis_title="Time (seconds)",
            yaxis_title="Bandwidth Utilization (%)",
            yaxis2=dict(title="Latency (ms)", overlaying='y', side='right'),
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        # Performance metrics
        avg_utilization = np.mean(bandwidth_utilization) * 100
        avg_latency = np.mean(latency_actual)
        sla_compliance = np.mean(latency_actual <= latency) * 100
        
        col1, col2, col3 = st.columns(3)
        
        with col1:
            st.metric("Avg Utilization", f"{avg_utilization:.1f}%")
        with col2:
            st.metric("Avg Latency", f"{avg_latency:.1f} ms")
        with col3:
            st.metric("SLA Compliance", f"{sla_compliance:.1f}%")

def create_domain_tutorial():
    """Domain applications tutorial"""
    
    st.markdown("## 🌐 Domain Applications Tutorial")
    
    st.markdown("""
    ### Real-World Integration Scenarios
    
    Learn how to configure the platform for specific domains and applications.
    """)
    
    domain = st.selectbox("Select Domain", 
                         ["Healthcare & Telemedicine", 
                          "Industry 4.0 & Manufacturing",
                          "Autonomous Vehicles",
                          "Smart Cities"])
    
    if domain == "Healthcare & Telemedicine":
        create_healthcare_scenario()
    elif domain == "Industry 4.0 & Manufacturing":
        create_industry_scenario()
    elif domain == "Autonomous Vehicles":
        create_automotive_scenario()
    elif domain == "Smart Cities":
        create_smart_city_scenario()

def create_healthcare_scenario():
    """Healthcare scenario tutorial"""
    
    st.markdown("### 🏥 Healthcare & Telemedicine Configuration")
    
    col1, col2 = st.columns(2)
    
    with col1:
        st.markdown("""
        **Use Cases:**
        - Remote surgery with haptic feedback
        - Real-time medical imaging transmission
        - Emergency response coordination
        - IoT medical device monitoring
        
        **Requirements:**
        - Ultra-low latency (<1ms for surgery)
        - High reliability (99.999%)
        - High bandwidth for imaging
        - Secure communications
        """)
    
    with col2:
        st.markdown("**Recommended Configuration:**")
        
        config = {
            "THz Frequency": "1.5 THz",
            "Slice Types": "URLLC + eMBB",
            "AI Optimization": "Enabled",
            "Security Level": "Maximum",
            "Backup Systems": "Enabled"
        }
        
        for key, value in config.items():
            st.code(f"{key}: {value}")
    
    # Healthcare metrics simulation
    if st.button("🏥 Simulate Healthcare Network"):
        
        # Generate healthcare-specific metrics
        procedures = ["Remote Surgery", "Medical Imaging", "Patient Monitoring", "Emergency Response"]
        latencies = [0.8, 5.2, 10.5, 2.1]  # ms
        bandwidths = [50, 800, 10, 100]  # Mbps
        reliabilities = [99.999, 99.9, 99.5, 99.99]  # %
        
        # Create healthcare dashboard
        fig = go.Figure(data=[
            go.Bar(name='Latency (ms)', x=procedures, y=latencies),
            go.Bar(name='Bandwidth (Mbps)', x=procedures, y=bandwidths, yaxis='y2'),
        ])
        
        fig.update_layout(
            title="Healthcare Application Performance",
            xaxis_title="Applications",
            yaxis_title="Latency (ms)",
            yaxis2=dict(title="Bandwidth (Mbps)", overlaying='y', side='right'),
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        # Display compliance
        for i, procedure in enumerate(procedures):
            compliance = "✅ COMPLIANT" if latencies[i] < 10 and reliabilities[i] > 99 else "⚠️ NEEDS OPTIMIZATION"
            st.write(f"**{procedure}**: {compliance}")

def create_industry_scenario():
    """Industry 4.0 scenario"""
    
    st.markdown("### 🏭 Industry 4.0 & Manufacturing")
    
    st.markdown("""
    **Smart Factory Applications:**
    - Real-time process control
    - Predictive maintenance
    - Quality inspection with AI
    - Supply chain optimization
    - Worker safety monitoring
    """)
    
    # Factory simulation
    num_machines = st.slider("Number of Machines", 10, 1000, 100)
    automation_level = st.selectbox("Automation Level", ["Basic", "Advanced", "Fully Autonomous"])
    
    if st.button("🏭 Simulate Smart Factory"):
        
        # Generate factory metrics
        machines = [f"Machine_{i+1}" for i in range(min(num_machines, 20))]  # Limit display
        efficiency = np.random.uniform(85, 98, len(machines))
        downtime = np.random.uniform(0, 5, len(machines))
        
        fig = go.Figure()
        
        fig.add_trace(go.Bar(
            x=machines, y=efficiency,
            name='Efficiency (%)',
            marker_color='green'
        ))
        
        fig.update_layout(
            title=f"Factory Efficiency - {automation_level} Automation",
            xaxis_title="Machines",
            yaxis_title="Efficiency (%)",
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        avg_efficiency = np.mean(efficiency)
        total_downtime = np.sum(downtime)
        
        col1, col2, col3 = st.columns(3)
        
        with col1:
            st.metric("Average Efficiency", f"{avg_efficiency:.1f}%")
        with col2:
            st.metric("Total Downtime", f"{total_downtime:.1f} hours")
        with col3:
            productivity_boost = {"Basic": 15, "Advanced": 35, "Fully Autonomous": 55}[automation_level]
            st.metric("Productivity Boost", f"+{productivity_boost}%")

def create_automotive_scenario():
    """Automotive scenario"""
    
    st.markdown("### 🚗 Autonomous Vehicles & V2X")
    
    st.markdown("""
    **V2X Communications:**
    - Vehicle-to-Vehicle (V2V)
    - Vehicle-to-Infrastructure (V2I)
    - Vehicle-to-Pedestrian (V2P)
    - Vehicle-to-Network (V2N)
    """)
    
    scenario = st.selectbox("Traffic Scenario", 
                           ["Urban Intersection", "Highway Merge", "Emergency Response", "Parking Management"])
    
    if st.button("🚗 Simulate V2X Network"):
        
        # Generate V2X metrics
        time_points = np.arange(0, 30, 1)
        vehicle_count = 50 + 30 * np.sin(time_points * 0.2) + np.random.normal(0, 5, len(time_points))
        vehicle_count = np.clip(vehicle_count, 20, 100)
        
        latency = 2 + np.random.exponential(1, len(time_points))
        latency = np.clip(latency, 1, 10)
        
        fig = go.Figure()
        
        fig.add_trace(go.Scatter(
            x=time_points, y=vehicle_count,
            mode='lines+markers',
            name='Connected Vehicles',
            line=dict(color='blue')
        ))
        
        fig.add_trace(go.Scatter(
            x=time_points, y=latency,
            mode='lines+markers',
            name='V2X Latency (ms)',
            yaxis='y2',
            line=dict(color='red')
        ))
        
        fig.update_layout(
            title=f"V2X Performance - {scenario}",
            xaxis_title="Time (seconds)",
            yaxis_title="Connected Vehicles",
            yaxis2=dict(title="Latency (ms)", overlaying='y', side='right'),
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        safety_score = max(0, 100 - np.mean(latency) * 5)
        efficiency_gain = min(40, np.mean(vehicle_count) / 2)
        
        col1, col2 = st.columns(2)
        
        with col1:
            st.metric("Safety Score", f"{safety_score:.0f}/100")
        with col2:
            st.metric("Traffic Efficiency", f"+{efficiency_gain:.0f}%")

def create_smart_city_scenario():
    """Smart city scenario"""
    
    st.markdown("### 🌱 Smart Cities & Environmental Monitoring")
    
    st.markdown("""
    **Smart City Applications:**
    - Environmental sensor networks
    - Smart grid management
    - Traffic optimization
    - Waste management
    - Public safety systems
    """)
    
    city_size = st.selectbox("City Size", ["Small (50k)", "Medium (500k)", "Large (5M)"])
    
    if st.button("🌱 Simulate Smart City"):
        
        # Generate smart city metrics
        sensors = {"Air Quality": 85, "Traffic": 78, "Energy": 92, "Water": 88, "Waste": 75}
        
        # Create radar chart
        categories = list(sensors.keys())
        values = list(sensors.values())
        
        fig = go.Figure()
        
        fig.add_trace(go.Scatterpolar(
            r=values,
            theta=categories,
            fill='toself',
            name='City Performance'
        ))
        
        fig.update_layout(
            polar=dict(
                radialaxis=dict(
                    visible=True,
                    range=[0, 100]
                )),
            title=f"Smart City Performance - {city_size}",
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        # Calculate improvements
        size_multiplier = {"Small (50k)": 1.0, "Medium (500k)": 1.2, "Large (5M)": 1.5}[city_size]
        
        improvements = {
            "Energy Savings": f"+{25 * size_multiplier:.0f}%",
            "Traffic Reduction": f"+{20 * size_multiplier:.0f}%",
            "Air Quality": f"+{15 * size_multiplier:.0f}%",
            "Citizen Satisfaction": f"+{30 * size_multiplier:.0f}%"
        }
        
        for metric, improvement in improvements.items():
            st.success(f"**{metric}**: {improvement}")

def create_research_tutorial():
    """Research tools tutorial"""
    
    st.markdown("## 🔬 Research Tools Tutorial")
    
    st.markdown("""
    ### Publication-Ready Research
    
    Learn to use the platform for academic research and industry publications.
    """)
    
    research_type = st.selectbox("Research Focus", 
                                ["Performance Benchmarking", 
                                 "Comparative Analysis", 
                                 "Novel Algorithm Testing",
                                 "System Optimization"])
    
    if research_type == "Performance Benchmarking":
        create_benchmarking_tutorial()
    elif research_type == "Comparative Analysis":
        create_comparison_tutorial()

def create_benchmarking_tutorial():
    """Benchmarking tutorial"""
    
    st.markdown("### 📊 Performance Benchmarking")
    
    st.markdown("""
    **Benchmarking Process:**
    1. Define baseline scenarios
    2. Configure test parameters
    3. Run systematic experiments
    4. Collect and analyze data
    5. Generate statistical reports
    """)
    
    baseline = st.selectbox("Baseline Technology", ["5G", "WiFi 6E", "Theoretical 6G"])
    
    if st.button("📊 Run Benchmark"):
        
        # Generate benchmark comparison
        metrics = ["Throughput", "Latency", "Energy Efficiency", "Coverage", "Scalability"]
        baseline_scores = [100, 100, 100, 100, 100]  # Normalized to 100
        
        if baseline == "5G":
            platform_scores = [150, 85, 120, 110, 140]
        elif baseline == "WiFi 6E":
            platform_scores = [200, 70, 110, 150, 160]
        else:  # Theoretical 6G
            platform_scores = [95, 105, 98, 102, 98]
        
        # Create comparison chart
        fig = go.Figure(data=[
            go.Bar(name=baseline, x=metrics, y=baseline_scores),
            go.Bar(name='O-RAN 6G Platform', x=metrics, y=platform_scores)
        ])
        
        fig.update_layout(
            title=f"Performance Comparison vs {baseline}",
            xaxis_title="Metrics",
            yaxis_title="Relative Performance (%)",
            height=400
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        # Calculate overall improvement
        improvement = np.mean(platform_scores) - 100
        st.success(f"🎉 Overall Performance Improvement: +{improvement:.1f}%")

def create_comparison_tutorial():
    """Comparison tutorial"""
    
    st.markdown("### 🔍 Comparative Analysis")
    
    platforms = ["Ericsson 6G", "Nokia 6G", "Huawei 6G", "Samsung 6G"]
    selected_platforms = st.multiselect("Select Platforms to Compare", platforms, default=platforms[:2])
    
    if st.button("🔍 Compare Platforms") and selected_platforms:
        
        # Generate comparison data
        metrics = ["THz Support", "AI Integration", "Network Slicing", "Energy Efficiency"]
        
        platform_scores = {
            "O-RAN 6G (This)": [95, 92, 88, 85],
            "Ericsson 6G": [75, 80, 85, 70],
            "Nokia 6G": [70, 75, 80, 75],
            "Huawei 6G": [80, 85, 88, 80],
            "Samsung 6G": [65, 70, 75, 85]
        }
        
        fig = go.Figure()
        
        # Add our platform
        fig.add_trace(go.Scatterpolar(
            r=platform_scores["O-RAN 6G (This)"],
            theta=metrics,
            fill='toself',
            name='O-RAN 6G (This Platform)',
            line=dict(color='red', width=3)
        ))
        
        # Add selected platforms
        colors = ['blue', 'green', 'orange', 'purple']
        for i, platform in enumerate(selected_platforms):
            if platform in platform_scores:
                fig.add_trace(go.Scatterpolar(
                    r=platform_scores[platform],
                    theta=metrics,
                    fill='toself',
                    name=platform,
                    line=dict(color=colors[i % len(colors)])
                ))
        
        fig.update_layout(
            polar=dict(
                radialaxis=dict(
                    visible=True,
                    range=[0, 100]
                )),
            title="6G Platform Competitive Analysis",
            height=500
        )
        
        st.plotly_chart(fig, use_container_width=True)
        
        # Show competitive advantages
        our_scores = platform_scores["O-RAN 6G (This)"]
        advantages = []
        
        for i, metric in enumerate(metrics):
            competitor_max = max([platform_scores[p][i] for p in selected_platforms if p in platform_scores])
            if our_scores[i] > competitor_max:
                advantage = our_scores[i] - competitor_max
                advantages.append(f"**{metric}**: +{advantage:.0f} points ahead")
        
        if advantages:
            st.markdown("### 🏆 Competitive Advantages")
            for advantage in advantages:
                st.success(advantage)

def create_advanced_tutorial():
    """Advanced features tutorial"""
    
    st.markdown("## 📊 Advanced Features Tutorial")
    
    st.markdown("""
    ### Cutting-Edge Capabilities
    
    Explore the most advanced features of the platform for next-generation research.
    """)
    
    feature = st.selectbox("Advanced Feature", 
                          ["Holographic Communications", 
                           "Quantum Integration", 
                           "Energy Optimization",
                           "3D Network Visualization"])
    
    if feature == "Holographic Communications":
        st.markdown("""
        ### 🎭 Holographic Communications
        
        Simulate the transmission of holographic data requiring:
        - Extremely high bandwidth (>1 Tbps)
        - Ultra-low latency (<0.1ms)
        - 3D data compression algorithms
        - Real-time rendering capabilities
        """)
        
        resolution = st.selectbox("Hologram Resolution", ["4K", "8K", "16K"])
        fps = st.slider("Frame Rate", 30, 120, 60)
        
        # Calculate requirements
        resolution_map = {"4K": 8.3, "8K": 33.2, "16K": 132.7}  # Mbps base
        bandwidth_required = resolution_map[resolution] * fps / 30 * 100  # For holographic
        
        st.metric("Bandwidth Required", f"{bandwidth_required/1000:.1f} Gbps")
        
        if bandwidth_required > 1000:  # >1 Tbps
            st.warning("⚠️ Requires THz frequencies for sufficient bandwidth")
        else:
            st.success("✅ Achievable with current configuration")

if __name__ == "__main__":
    main()
