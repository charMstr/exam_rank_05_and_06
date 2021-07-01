/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Warlock.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 04:26:51 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 04:41:21 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Warlock.hpp"

#include <iostream>

//constructor
Warlock::Warlock(std::string const &name, std::string const &title) :
	name(name),
	title(title)
{
	std::cout << this->name << ": This looks like another boring day." << std::endl;
}

//destructor
Warlock::~Warlock(void)
{
	std::cout << this->name << ": My job here is done!" << std::endl;
}

std::string const &Warlock::getName(void) const
{ return this->name; }

std::string const &Warlock::getTitle(void) const
{ return this->title; }

void	Warlock::setTitle(std::string const &title)
{ this->title = title; }

void Warlock::introduce() const
{
	std::cout << this->name << ": I am " << this->name << ", " << this->title << "!" << std::endl;
}
