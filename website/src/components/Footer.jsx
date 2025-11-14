import React from 'react';
import { Github, Mail, ExternalLink } from 'lucide-react';
import '../styles/footer.css';

const Footer = () => {
  return (
    <footer className="footer">
      <div className="footer-content">
        <div className="footer-left">
          <div className="footer-brand">NAVRA</div>
          <p className="footer-tagline">Campus Route Optimizer</p>
        </div>

        <div className="footer-links">
          <a href="/">Home</a>
          <a href="/optimizer">Optimizer</a>
          <a href="/algorithms">Algorithms</a>
          <a href="/about">About</a>
        </div>

        <div className="footer-social">
          <a href="https://github.com" target="_blank" rel="noopener noreferrer" aria-label="GitHub">
            <Github size={18} />
          </a>
          <a href="mailto:navra@iitj.ac.in" aria-label="Email">
            <Mail size={18} />
          </a>
          <a href="https://iitj.ac.in" target="_blank" rel="noopener noreferrer" aria-label="IIT Jodhpur">
            <ExternalLink size={18} />
          </a>
        </div>
      </div>

      <div className="footer-bottom">
        <p>&copy; 2025 NAVRA • DSA Course Project • IIT Jodhpur</p>
      </div>
    </footer>
  );
};

export default Footer;
