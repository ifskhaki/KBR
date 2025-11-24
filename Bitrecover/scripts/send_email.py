#!/usr/bin/env python3
"""
Email notification script for Bitrecover
Handles both startup and match notifications
"""

import smtplib
import sys
from email.mime.text import MIMEText
from datetime import datetime
import json
import os

# Default email configuration
DEFAULT_CONFIG = {
    "smtp_server": "smtp.gmail.com",
    "smtp_port": 587,
    "username": "",
    "password": "",
    "recipients": []
}

def load_config():
    """Load email configuration from config file"""
    config_paths = [
        "config/config.json",
        "../config/config.json",
        os.path.expanduser("~/.bitrecover/config.json")
    ]
    
    for path in config_paths:
        if os.path.exists(path):
            try:
                with open(path, 'r') as f:
                    config = json.load(f)
                    if 'email' in config:
                        return config['email']
            except Exception as e:
                print(f"Warning: Could not load config from {path}: {e}")
    
    return DEFAULT_CONFIG

def send_email(subject, body, config):
    """Send email notification"""
    if not config.get('enabled', False):
        print("Email notifications disabled")
        return
    
    if not config.get('username') or not config.get('password'):
        print("Warning: Email credentials not configured")
        return
    
    try:
        msg = MIMEText(body, 'plain', 'utf-8')
        msg["Subject"] = subject
        msg["From"] = config['username']
        msg["To"] = ", ".join(config.get('recipients', []))
        
        with smtplib.SMTP(config.get('smtp_server', 'smtp.gmail.com'), 
                          config.get('smtp_port', 587)) as server:
            server.starttls()
            server.login(config['username'], config['password'])
            server.sendmail(config['username'], 
                          config.get('recipients', []), 
                          msg.as_string())
        print(f"[+] Email sent: {subject}")
    except Exception as e:
        print(f"[-] Email failed: {e}")

def send_startup_notification(sys_info):
    """Send startup notification"""
    config = load_config()
    
    subject = "🚀 Bitrecover Started - Multi-GPU Pre-2012 Search"
    
    body = f"""Bitrecover Multi-GPU Bitcoin Key Finder Started!

⏰ Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
🖥️  Host: {sys_info.get('hostname', 'unknown')}
💻 OS: {sys_info.get('os', 'unknown')}
🎯 Target: Pre-2012 Bitcoin wallets (Random 256-bit keys)
⚡ GPUs: {sys_info.get('gpu_count', 0)} device(s) detected

GPU Details:
"""
    
    for i, gpu in enumerate(sys_info.get('gpus', [])):
        body += f"  GPU {i}: {gpu.get('name', 'Unknown')} ({gpu.get('memory_mb', 0)} MB)\n"
    
    body += f"""
📁 Targets: {sys_info.get('targets_file', 'address.txt')}
💾 Output: {sys_info.get('output_file', 'Success.txt')}

Starting parallel search across all GPUs...
"""
    
    send_email(subject, body, config)

def send_match_notification(match_info):
    """Send match notification"""
    config = load_config()
    
    address = match_info.get('address', '')
    subject = f"🎉 BITCOIN MATCH! - {address[:10]}..."
    
    body = f"""🚀 BITCOIN MATCH FOUND! 🚀

⏰ Time: {match_info.get('timestamp', datetime.now().strftime('%Y-%m-%d %H:%M:%S'))}
📍 Address: {match_info.get('address', '')}
🔑 Private Key: {match_info.get('private_key', '')}
💳 WIF: {match_info.get('wif', '')}
🎮 GPU: {match_info.get('gpu_id', 'unknown')}

💰 Pre-2012 wallet found!
"""
    
    send_email(subject, body, config)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        # Startup notification
        sys_info = {
            'hostname': sys.argv[1] if len(sys.argv) > 1 else 'unknown',
            'os': sys.argv[2] if len(sys.argv) > 2 else 'unknown',
            'gpu_count': int(sys.argv[3]) if len(sys.argv) > 3 else 0,
            'gpus': []
        }
        
        # Parse GPU info
        idx = 4
        for i in range(sys_info['gpu_count']):
            if idx + 1 < len(sys.argv):
                gpu = {
                    'name': sys.argv[idx],
                    'memory_mb': int(sys.argv[idx + 1]) if idx + 1 < len(sys.argv) else 0
                }
                sys_info['gpus'].append(gpu)
                idx += 2
        
        send_startup_notification(sys_info)
    else:
        # Match notification
        if len(sys.argv) >= 4:
            match_info = {
                'address': sys.argv[1],
                'private_key': sys.argv[2],
                'wif': sys.argv[3],
                'gpu_id': sys.argv[4] if len(sys.argv) > 4 else 'unknown',
                'timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            }
            send_match_notification(match_info)
        else:
            print("Usage: python send_email.py [address] [private_key] [wif] [gpu_id]")
            print("   or: python send_email.py [startup args...]")

